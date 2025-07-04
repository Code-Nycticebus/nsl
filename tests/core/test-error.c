#include "nsl/core/error.h"

typedef enum {
    TEST_ERROR = 1,
} TestError;

static i32 function_that_fails(nsl_Error *error) {
    NSL_ERROR_EMIT(error, TEST_ERROR, "function that fails fails!?!");
    return 0;
}

static void test_emit(void) {
    nsl_Error error = {0};
    function_that_fails(&error);
    NSL_ASSERT(error.code != 0 && "The error code should not be '0'");
    NSL_ASSERT(error.file && "The 'file' was not set");
    NSL_ASSERT(error.line && "The 'line' was not set");
    NSL_ASSERT(error.func && "The 'func' was not set");
    NSL_ASSERT(error.message && "The 'message' was not set");
}

static void test_propagate(void) {
    nsl_Error error = {0};
    function_that_fails(&error);
    NSL_ERROR_PROPAGATE(&error, {
        NSL_ERROR_EMIT(&error, -2, "different message"); 
    });
    NSL_ASSERT(error.code == -2 && "The error code was not changed!");
}

static void test_handle(void) {
    bool handled = false;
    nsl_Error error = {0};
    function_that_fails(&error);
    NSL_ERROR_HANDLE(&error, {
        switch ((TestError)error.code) {
            NSL_ERROR_EXCEPT_CASE(&error, TEST_ERROR, { handled = true; });
        }
    });
    NSL_ASSERT(handled && "the error was not handled correctly");
}

void run_test_error(void);
void run_test_error(void) {
    test_emit();
    test_propagate();
    test_handle();
}
