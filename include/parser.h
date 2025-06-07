#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// ASTڵ
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DECL,
    AST_VAR_DECL,
    AST_ASSIGNMENT,
    AST_BINARY_OP,
    AST_NUMBER,
    AST_VARIABLE,
    AST_RETURN,
    AST_IF,
    AST_WHILE,
    AST_COMPOUND_STMT,
    AST_PARAM_LIST,
    AST_ARG_LIST
} ASTNodeType;

// ASTڵṹ
typedef struct ASTNode {
    ASTNodeType type;
    char value[MAX_TOKEN_LEN];

    struct ASTNode* left;
    struct ASTNode* right;

    struct ASTNode** children;
    int child_count;
} ASTNode;

// ﷨ṹ
typedef struct {
    Token current_token;
    FILE* source_file;
} Parser;

// 
void init_parser(Parser* parser, FILE* file);
ASTNode* parse(Parser* parser);
void free_ast(ASTNode* node);

#endif
