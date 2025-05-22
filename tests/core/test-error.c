#include "nc/core/error.h"

#include <assert.h>

typedef enum {
    TEST_ERROR = 1,
} TestError;

static i32 function_that_fails(nc_Error *error) {
    NC_ERROR_EMIT(error, TEST_ERROR, "function that fails fails!?!");
    return 0;
}

static void test_emit() {
    nc_Error error = {0};
    function_that_fails(&error);
    assert(error.code != 0 && "The error code should not be '0'");
    assert(error.file && "The 'file' was not set");
    assert(error.line && "The 'line' was not set");
    assert(error.func && "The 'func' was not set");
    assert(error.message && "The 'message' was not set");
}

static void test_propagate() {
    nc_Error error = {0};
    function_that_fails(&error);
    NC_ERROR_PROPAGATE(&error, {
        NC_ERROR_EMIT(&error, -2, "different message"); 
    });
    assert(error.code == -2 && "The error code was not changed!");
}

static void test_handle() {
    bool handled = false;
    nc_Error error = {0};
    function_that_fails(&error);
    NC_ERROR_HANDLE(&error, {
        switch ((TestError)error.code) {
            NC_ERROR_EXCEPT_CASE(&error, TEST_ERROR, { handled = true; });
        }
    });
    assert(handled && "the error was not handled correctly");
}

void run_test_error() {
    test_emit();
    test_propagate();
    test_handle();
}
