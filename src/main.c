#include <stdio.h>
#include "lexer.h"
#include "parser.h"

void print_ast(ASTNode* node, int level) {
    if (node == NULL) return;

    // ʾ㼶
    for (int i = 0; i < level; i++) printf("  ");

    // ӡڵͺֵ
    const char* node_types[] = {
        "PROGRAM", "FUNCTION_DECL", "VAR_DECL", "ASSIGNMENT", "BINARY_OP",
        "NUMBER", "VARIABLE", "RETURN", "IF", "WHILE", "COMPOUND_STMT",
        "PARAM_LIST", "ARG_LIST"
    };

    printf("[%s] %s\n", node_types[node->type], node->value);

    // ݹӡӽڵ
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

    // ʷ
    printf("=== Lexical Analysis ===\n");
    Token token;
    do {
        token = get_next_token(file);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    // ļָ
    rewind(file);

    // ﷨
    printf("\n=== Syntax Analysis ===\n");
    Parser parser;
    init_parser(&parser, file);

    ASTNode* ast = parse(&parser);
    printf("\n=== Abstract Syntax Tree ===\n");
    print_ast(ast, 0);

    // Դ
    free_ast(ast);
    fclose(file);
    return 0;
}
