#include "lexer.h"

// 判断是否为关键字
static int is_keyword(const char* str) {
    for (int i = 0; keywords[i]; ++i) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// 判断字符是否为运算符
static int is_operator_char(char c) {
    return strchr("+-*/=<>!&|", c) != NULL;
}

// 判断字符是否为分隔符
static int is_separator(char c) {
    return strchr("(){};,[]", c) != NULL;
}

// 当前行号
static int current_line = 1;

// 跳过空白并更新行号
static void skip_whitespace(FILE* file) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            current_line++;
        } else if (!isspace(c)) {
            ungetc(c, file);
            break;
        }
    }
}

// 读取一个Token
Token get_next_token(FILE* file) {
    Token token = { TOKEN_UNKNOWN, "", 0 };
    skip_whitespace(file);
    int c = fgetc(file);
    token.line = current_line;
    if (c == EOF) {
        token.type = TOKEN_EOF;
        return token;
    }

    if (isalpha(c) || c == '_') {
        int i = 0;
        token.value[i++] = c;
        while (isalnum(c = fgetc(file)) || c == '_') {
            token.value[i++] = c;
        }
        token.value[i] = '\0';
        ungetc(c, file);
        token.type = is_keyword(token.value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    } else if (isdigit(c)) {
        int i = 0;
        token.value[i++] = c;
        while (isdigit(c = fgetc(file))) {
            token.value[i++] = c;
        }
        token.value[i] = '\0';
        ungetc(c, file);
        token.type = TOKEN_NUMBER;
    } else if (is_operator_char(c)) {
        int i = 0;
        token.value[i++] = c;
        int next = fgetc(file);
        if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
            (c == '<' && next == '=') || (c == '>' && next == '=')) {
            token.value[i++] = next;
        } else {
            ungetc(next, file);
        }
        token.value[i] = '\0';
        token.type = TOKEN_OPERATOR;
    } else if (is_separator(c)) {
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_SEPARATOR;
    } else {
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_UNKNOWN;
        if (error_file) {
            fprintf(error_file, "Line %d: Unknown token '%c'\n", token.line, c);
            error_count++;
        }
    }

    return token;
}

// 打印Token信息
void print_token(Token token) {
    const char* type_str[] = {
        "KEYWORD", "IDENTIFIER", "NUMBER", "OPERATOR", "SEPARATOR", "UNKNOWN", "EOF" };
    printf("<%s, \"%s\">\n", type_str[token.type], token.value);
}
