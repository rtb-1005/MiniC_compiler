#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 解析错误处理
static void parser_error(const char* message) {
    fprintf(stderr, "Parser Error: %s\n", message);
    exit(1);
}

// 消耗当前token并读取下一个
static void eat(Parser* parser, TokenType expected_type) {
    if (parser->current_token.type == expected_type) {
        parser->current_token = get_next_token(parser->source_file);
    } else {
        char msg[100];
        sprintf(msg, "Expected token type %d, got %d", expected_type,
                parser->current_token.type);
        parser_error(msg);
    }
}

// 初始化解析器
void init_parser(Parser* parser, FILE* file) {
    parser->source_file = file;
    parser->current_token = get_next_token(file);
}

// ===================== 辅助函数声明 =====================
static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_function(Parser* parser);
static ASTNode* parse_compound(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_expression(Parser* parser);

// 创建节点
static ASTNode* create_node(ASTNodeType type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->type = type;
    if (value)
        strncpy(node->value, value, MAX_TOKEN_LEN - 1);
    else
        node->value[0] = '\0';
    node->value[MAX_TOKEN_LEN - 1] = '\0';
    node->left = node->right = NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

// ===================== 语法规则实现 =====================

// 解析入口
ASTNode* parse(Parser* parser) { return parse_program(parser); }

// program ::= (function_decl)*
static ASTNode* parse_program(Parser* parser) {
    ASTNode* prog = create_node(AST_PROGRAM, "");
    while (parser->current_token.type != TOKEN_EOF) {
        prog->child_count++;
        prog->children = realloc(prog->children,
                                 sizeof(ASTNode*) * prog->child_count);
        prog->children[prog->child_count - 1] = parse_function(parser);
    }
    return prog;
}

// function_decl ::= 'int' IDENTIFIER '(' ')' compound_stmt
static ASTNode* parse_function(Parser* parser) {
    if (parser->current_token.type != TOKEN_KEYWORD ||
        strcmp(parser->current_token.value, "int") != 0)
        parser_error("function must start with 'int'");
    eat(parser, TOKEN_KEYWORD);

    if (parser->current_token.type != TOKEN_IDENTIFIER)
        parser_error("expect function name");
    char name[MAX_TOKEN_LEN];
    strncpy(name, parser->current_token.value, MAX_TOKEN_LEN);
    eat(parser, TOKEN_IDENTIFIER);

    eat(parser, TOKEN_SEPARATOR); // (
    eat(parser, TOKEN_SEPARATOR); // )

    ASTNode* func = create_node(AST_FUNCTION_DECL, name);
    func->left = parse_compound(parser);
    return func;
}

// compound_stmt ::= '{' statement* '}'
static ASTNode* parse_compound(Parser* parser) {
    if (parser->current_token.type != TOKEN_SEPARATOR ||
        strcmp(parser->current_token.value, "{") != 0)
        parser_error("expected '{'");
    eat(parser, TOKEN_SEPARATOR);

    ASTNode* node = create_node(AST_COMPOUND_STMT, "");
    while (!(parser->current_token.type == TOKEN_SEPARATOR &&
             strcmp(parser->current_token.value, "}") == 0)) {
        node->child_count++;
        node->children = realloc(node->children,
                                 sizeof(ASTNode*) * node->child_count);
        node->children[node->child_count - 1] = parse_statement(parser);
    }
    eat(parser, TOKEN_SEPARATOR); // }
    return node;
}

// if_stmt ::= 'if' '(' expression ')' statement ('else' statement)?
static ASTNode* parse_if(Parser* parser) {
    eat(parser, TOKEN_KEYWORD); // if
    eat(parser, TOKEN_SEPARATOR); // (
    ASTNode* cond = parse_expression(parser);
    eat(parser, TOKEN_SEPARATOR); // )

    ASTNode* node = create_node(AST_IF, "");
    node->left = cond;
    node->child_count = 1;
    node->children = malloc(sizeof(ASTNode*));
    node->children[0] = parse_statement(parser);

    if (parser->current_token.type == TOKEN_KEYWORD &&
        strcmp(parser->current_token.value, "else") == 0) {
        eat(parser, TOKEN_KEYWORD);
        node->child_count = 2;
        node->children = realloc(node->children, sizeof(ASTNode*) * 2);
        node->children[1] = parse_statement(parser);
    }
    return node;
}

// while_stmt ::= 'while' '(' expression ')' statement
static ASTNode* parse_while(Parser* parser) {
    eat(parser, TOKEN_KEYWORD); // while
    eat(parser, TOKEN_SEPARATOR); // (
    ASTNode* cond = parse_expression(parser);
    eat(parser, TOKEN_SEPARATOR); // )

    ASTNode* node = create_node(AST_WHILE, "");
    node->left = cond;
    node->right = parse_statement(parser);
    return node;
}

// variable_decl ::= 'int' IDENTIFIER ('=' expression)? ';'
static ASTNode* parse_var_decl(Parser* parser) {
    eat(parser, TOKEN_KEYWORD); // int

    if (parser->current_token.type != TOKEN_IDENTIFIER)
        parser_error("expect variable name");
    char name[MAX_TOKEN_LEN];
    strncpy(name, parser->current_token.value, MAX_TOKEN_LEN);
    eat(parser, TOKEN_IDENTIFIER);

    ASTNode* node = create_node(AST_VAR_DECL, name);

    if (parser->current_token.type == TOKEN_OPERATOR &&
        strcmp(parser->current_token.value, "=") == 0) {
        eat(parser, TOKEN_OPERATOR);
        node->left = parse_expression(parser);
    }

    if (parser->current_token.type != TOKEN_SEPARATOR ||
        strcmp(parser->current_token.value, ";") != 0)
        parser_error("expected ';'");
    eat(parser, TOKEN_SEPARATOR);
    return node;
}

// assignment ::= IDENTIFIER '=' expression ';'
static ASTNode* parse_assignment(Parser* parser) {
    if (parser->current_token.type != TOKEN_IDENTIFIER)
        parser_error("expect variable name");
    char name[MAX_TOKEN_LEN];
    strncpy(name, parser->current_token.value, MAX_TOKEN_LEN);
    eat(parser, TOKEN_IDENTIFIER);

    if (parser->current_token.type != TOKEN_OPERATOR ||
        strcmp(parser->current_token.value, "=") != 0)
        parser_error("expected '='");
    eat(parser, TOKEN_OPERATOR);

    ASTNode* node = create_node(AST_ASSIGNMENT, name);
    node->left = parse_expression(parser);

    if (parser->current_token.type != TOKEN_SEPARATOR ||
        strcmp(parser->current_token.value, ";") != 0)
        parser_error("expected ';'");
    eat(parser, TOKEN_SEPARATOR);
    return node;
}

// return_stmt ::= 'return' expression ';'
static ASTNode* parse_return(Parser* parser) {
    eat(parser, TOKEN_KEYWORD);
    ASTNode* node = create_node(AST_RETURN, "");
    node->left = parse_expression(parser);

    if (parser->current_token.type != TOKEN_SEPARATOR ||
        strcmp(parser->current_token.value, ";") != 0)
        parser_error("expected ';'");
    eat(parser, TOKEN_SEPARATOR);
    return node;
}

// 根据当前token解析不同语句
static ASTNode* parse_statement(Parser* parser) {
    if (parser->current_token.type == TOKEN_KEYWORD) {
        if (strcmp(parser->current_token.value, "int") == 0)
            return parse_var_decl(parser);
        if (strcmp(parser->current_token.value, "return") == 0)
            return parse_return(parser);
        if (strcmp(parser->current_token.value, "if") == 0)
            return parse_if(parser);
        if (strcmp(parser->current_token.value, "while") == 0)
            return parse_while(parser);
    }

    if (parser->current_token.type == TOKEN_SEPARATOR &&
        strcmp(parser->current_token.value, "{") == 0)
        return parse_compound(parser);

    return parse_assignment(parser);
}

// -------- 表达式解析 --------

// factor ::= NUMBER | IDENTIFIER | '(' expression ')'
static ASTNode* parse_factor(Parser* parser) {
    if (parser->current_token.type == TOKEN_NUMBER) {
        ASTNode* node = create_node(AST_NUMBER, parser->current_token.value);
        eat(parser, TOKEN_NUMBER);
        return node;
    } else if (parser->current_token.type == TOKEN_IDENTIFIER) {
        ASTNode* node = create_node(AST_VARIABLE, parser->current_token.value);
        eat(parser, TOKEN_IDENTIFIER);
        return node;
    } else if (parser->current_token.type == TOKEN_SEPARATOR &&
               strcmp(parser->current_token.value, "(") == 0) {
        eat(parser, TOKEN_SEPARATOR); // (
        ASTNode* node = parse_expression(parser);
        if (parser->current_token.type != TOKEN_SEPARATOR ||
            strcmp(parser->current_token.value, ")") != 0)
            parser_error("expected ')'");
        eat(parser, TOKEN_SEPARATOR);
        return node;
    }
    parser_error("invalid factor");
    return NULL;
}

// term ::= factor (('*' | '/') factor)*
static ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "*") == 0 ||
            strcmp(parser->current_token.value, "/") == 0)) {
        char op[MAX_TOKEN_LEN];
        strncpy(op, parser->current_token.value, MAX_TOKEN_LEN);
        eat(parser, TOKEN_OPERATOR);
        ASTNode* rhs = parse_factor(parser);
        ASTNode* new_node = create_node(AST_BINARY_OP, op);
        new_node->left = node;
        new_node->right = rhs;
        node = new_node;
    }
    return node;
}

// arith_expr ::= term (('+' | '-') term)*
static ASTNode* parse_arith_expr(Parser* parser) {
    ASTNode* node = parse_term(parser);
    while (parser->current_token.type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token.value, "+") == 0 ||
            strcmp(parser->current_token.value, "-") == 0)) {
        char op[MAX_TOKEN_LEN];
        strncpy(op, parser->current_token.value, MAX_TOKEN_LEN);
        eat(parser, TOKEN_OPERATOR);
        ASTNode* rhs = parse_term(parser);
        ASTNode* new_node = create_node(AST_BINARY_OP, op);
        new_node->left = node;
        new_node->right = rhs;
        node = new_node;
    }
    return node;
}

// comparison ::= arith_expr (relop arith_expr)?
static ASTNode* parse_comparison(Parser* parser) {
    ASTNode* node = parse_arith_expr(parser);
    if (parser->current_token.type == TOKEN_OPERATOR &&
        (strcmp(parser->current_token.value, "==") == 0 ||
         strcmp(parser->current_token.value, "!=") == 0 ||
         strcmp(parser->current_token.value, "<") == 0 ||
         strcmp(parser->current_token.value, ">") == 0 ||
         strcmp(parser->current_token.value, "<=") == 0 ||
         strcmp(parser->current_token.value, ">=") == 0)) {
        char op[MAX_TOKEN_LEN];
        strncpy(op, parser->current_token.value, MAX_TOKEN_LEN);
        eat(parser, TOKEN_OPERATOR);
        ASTNode* rhs = parse_arith_expr(parser);
        ASTNode* new_node = create_node(AST_BINARY_OP, op);
        new_node->left = node;
        new_node->right = rhs;
        node = new_node;
    }
    return node;
}

static ASTNode* parse_expression(Parser* parser) { return parse_comparison(parser); }

// 释放AST
void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    for (int i = 0; i < node->child_count; ++i)
        free_ast(node->children[i]);
    free(node->children);
    free(node);
}
