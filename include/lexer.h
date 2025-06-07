#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100  // ÿToken󳤶

// Minicֵ֧Ĺؼ
static const char* keywords[] = { "int", "float", "return", "if", "else", "while", "void", NULL };

// öToken
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

// Tokenṹ壬ͺֵ
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

// ʷ
Token get_next_token(FILE* file);
void print_token(Token token);

#endif
