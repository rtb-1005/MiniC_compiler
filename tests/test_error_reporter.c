#include <assert.h>
#include "error_reporter.h"

int main() {
    error_reporter_init();
    assert(!error_reporter_has_errors());

    error_reporter_add("first error");
    assert(error_reporter_has_errors());

    error_reporter_free();
    assert(!error_reporter_has_errors());

    return 0;
}
