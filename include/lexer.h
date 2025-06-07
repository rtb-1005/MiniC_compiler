#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100  // 每个Token的最大长度

// MiniC支持的关键字
static const char* keywords[] = { "int", "float", "return", "if", "else", "while", "void", NULL };

// 枚举Token
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

// Token结构体，存储类型和值
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

// 词法分析接口
Token get_next_token(FILE* file);
void print_token(Token token);

#endif
