#include <stdio.h>
#include "lexer.h"
#include "parser.h"

void print_ast(ASTNode* node, int level) {
    if (node == NULL) return;

    // 显示缩进级别
    for (int i = 0; i < level; i++) printf("  ");

    // 打印节点类型和值
    const char* node_types[] = {
        "PROGRAM", "FUNCTION_DECL", "VAR_DECL", "ASSIGNMENT", "BINARY_OP",
        "NUMBER", "VARIABLE", "RETURN", "IF", "WHILE", "COMPOUND_STMT",
        "PARAM_LIST", "ARG_LIST"
    };

    printf("[%s] %s\n", node_types[node->type], node->value);

    // 递归打印子节点
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);

    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], level + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open input file");
        return 1;
    }

    // 词法分析
    printf("=== Lexical Analysis ===\n");
    Token token;
    do {
        token = get_next_token(file);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    // 文件回到开头
    rewind(file);

    // 语法分析
    printf("\n=== Syntax Analysis ===\n");
    Parser parser;
    init_parser(&parser, file);

    ASTNode* ast = parse(&parser);
    printf("\n=== Abstract Syntax Tree ===\n");
    print_ast(ast, 0);

    // 释放资源
    free_ast(ast);
    fclose(file);
    return 0;
}
