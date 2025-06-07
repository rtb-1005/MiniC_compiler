#include "lexer.h"

// жһַǷǹؼ
static int is_keyword(const char* str) {
    for (int i = 0; keywords[i]; ++i) {
        if (strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// жǷǲֵַַ֧
static int is_operator_char(char c) {
    return strchr("+-*/=<>!&|", c) != NULL;
}

// жǷǷַָ
static int is_separator(char c) {
    return strchr("(){};,[]", c) != NULL;
}

// հַո񡢻СƱȣ
static void skip_whitespace(FILE* file) {
    int c;
    while (isspace(c = fgetc(file)));
    ungetc(c, file);  // ǿհַ˻ļ
}

// ʷȡһToken
Token get_next_token(FILE* file) {
    Token token = { TOKEN_UNKNOWN, "" };  // ʼToken
    skip_whitespace(file);  // հ

    int c = fgetc(file);
    if (c == EOF) {
        token.type = TOKEN_EOF;
        return token;
    }

    // ʶؼ֣ĸ»߿ͷ
    if (isalpha(c) || c == '_') {
        int i = 0;
        token.value[i++] = c;
        while (isalnum(c = fgetc(file)) || c == '_') {
            token.value[i++] = c;
        }
        token.value[i] = '\0';
        ungetc(c, file);

        // жǷΪؼ
        token.type = is_keyword(token.value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    }

    // ֻ֣
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

    // ֧˫ַ ==, !=, <=, >=
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

    // ָ ; , ( ) { } ȣ
    else if (is_separator(c)) {
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_SEPARATOR;
    }

    // δַ֪
    else {
        token.value[0] = c;
        token.value[1] = '\0';
        token.type = TOKEN_UNKNOWN;
    }

    return token;
}

// ӡTokenϢ
void print_token(Token token) {
    const char* type_str[] = {
        "KEYWORD", "IDENTIFIER", "NUMBER", "OPERATOR", "SEPARATOR", "UNKNOWN", "EOF"
    };
    printf("<%s, \"%s\">\n", type_str[token.type], token.value);
}
