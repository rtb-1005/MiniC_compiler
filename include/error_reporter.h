#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

void error_reporter_init();
void error_reporter_add(const char *msg);
void error_reporter_print();
int error_reporter_has_errors();
void error_reporter_free();

#endif // ERROR_REPORTER_H
