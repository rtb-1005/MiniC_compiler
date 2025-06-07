#include <stdio.h>
#include "lexer.h"
#include "parser.h"

// 递归打印AST
void print_ast(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");

    const char* node_types[] = {
        "PROGRAM", "FUNCTION_DECL", "VAR_DECL", "ASSIGNMENT", "BINARY_OP",
        "NUMBER", "VARIABLE", "RETURN", "IF", "WHILE", "COMPOUND_STMT",
        "PARAM_LIST", "ARG_LIST" };

    printf("[%s] %s\n", node_types[node->type], node->value);

    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
    for (int i = 0; i < node->child_count; i++)
        print_ast(node->children[i], level + 1);
}

FILE* error_file = NULL;  // 错误输出文件
int error_count = 0;      // 错误数量

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

    error_file = fopen("output_error", "w");
    if (!error_file) {
        perror("Failed to open output_error");
        fclose(file);
        return 1;
    }

    printf("=== Lexical Analysis ===\n");
    Token token;
    do {
        token = get_next_token(file);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    rewind(file);

    printf("\n=== Syntax Analysis ===\n");
    Parser parser;
    init_parser(&parser, file);

    ASTNode* ast = NULL;
    if (error_count == 0) {
        ast = parse(&parser);
        printf("\n=== Abstract Syntax Tree ===\n");
        print_ast(ast, 0);
    }

    if (ast) free_ast(ast);
    fclose(file);
    fclose(error_file);

    if (error_count == 0) {
        printf("Build Succeeded\n");
    } else {
        printf("Build Failed with %d error(s)\n", error_count);
        FILE* err = fopen("output_error", "r");
        if (err) {
            char line[256];
            while (fgets(line, sizeof(line), err))
                fputs(line, stdout);
            fclose(err);
        }
    }
    return error_count == 0 ? 0 : 1;
}
