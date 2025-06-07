#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100  // 单个Token最大长度

// MiniC 支持的关键字
static const char* keywords[] = { "int", "float", "return", "if", "else", "while", "void", NULL };

// Token 类型枚举
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

// Token 结构体，包含类型、值和行号
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;
} Token;

// 词法分析接口
Token get_next_token(FILE* file);
void print_token(Token token);

// 错误统计与输出文件
extern FILE* error_file;
extern int error_count;

#endif
