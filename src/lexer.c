#include "lexer.h"
#include "error_reporter.h"

// 判断字符串是否为关键字
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

// 跳过空格、制表符等空白字符
static void skip_whitespace(FILE* file) {
    int c;
    while (isspace(c = fgetc(file)));
    ungetc(c, file);  // 将读取的非空白字符放回文件
}

// 读取一个Token
Token get_next_token(FILE* file) {
    Token token = { TOKEN_UNKNOWN, "" };  // 初始化Token
    skip_whitespace(file);  // 跳过空白

    int c = fgetc(file);
    if (c == EOF) {
        token.type = TOKEN_EOF;
        return token;
    }

    // 识别关键字或标识符
    if (isalpha(c) || c == '_') {
        int i = 0;
        token.value[i++] = c;
        while (isalnum(c = fgetc(file)) || c == '_') {
            token.value[i++] = c;
        }
        token.value[i] = '\0';
        ungetc(c, file);

        // 判断是否为关键字
        token.type = is_keyword(token.value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    }

    // 识别数字
    else if (isdigit(c)) {
        int i = 0;
        token.value[i++] = c;
        while (isdigit(c = fgetc(file))) {
            token.value[i++] = c;
        }
        token.value[i] = '\0';
        ungetc(c, file);
        token.type = TOKEN_NUMBER;
    }

    // 支持双字符运算符 ==, !=, <=, >=
    else if (is_operator_char(c)) {
        int i = 0;
        token.value[i++] = c;
        int next = fgetc(file);
        if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
            (c == '<' && next == '=') || (c == '>' && next == '=')) {
            token.value[i++] = next;
        }
        else {
            ungetc(next, file);
        }
        token.value[i] = '\0';
        token.type = TOKEN_OPERATOR;
    }

    // 分隔符 ; , ( ) { } 等
    else if (is_separator(c)) {
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_SEPARATOR;
    }

    // 未知字符
    else {
        char msg[64];
        snprintf(msg, sizeof(msg), "Unknown token: %c", c);
        error_reporter_add(msg);
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_UNKNOWN;
    }

    return token;
}

// 打印Token信息
void print_token(Token token) {
    const char* type_str[] = {
        "KEYWORD", "IDENTIFIER", "NUMBER", "OPERATOR", "SEPARATOR", "UNKNOWN", "EOF"
    };
    printf("<%s, \"%s\">\n", type_str[token.type], token.value);
}
