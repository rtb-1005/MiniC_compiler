#include "error_reporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ErrorNode {
    char *msg;
    struct ErrorNode *next;
} ErrorNode;

static ErrorNode *head = NULL;
static ErrorNode *tail = NULL;
static int error_count = 0;

void error_reporter_init() {
    head = tail = NULL;
    error_count = 0;
}

void error_reporter_add(const char *msg) {
    ErrorNode *node = (ErrorNode *)malloc(sizeof(ErrorNode));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->msg = strdup(msg);
    node->next = NULL;
    if (!head) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    error_count++;
}

void error_reporter_print() {
    ErrorNode *cur = head;
    while (cur) {
        fprintf(stderr, "%s\n", cur->msg);
        cur = cur->next;
    }
}

int error_reporter_has_errors() {
    return error_count > 0;
}

void error_reporter_free() {
    ErrorNode *cur = head;
    while (cur) {
        ErrorNode *next = cur->next;
        free(cur->msg);
        free(cur);
        cur = next;
    }
    head = tail = NULL;
    error_count = 0;
}
