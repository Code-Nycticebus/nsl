#include "nsl.h"

#include "nsl.h"
#include <stdio.h>

static void test_nc_path_eq(void) {
    NSL_ASSERT(nsl_path_eq(NSL_PATH("test/test.txt"), NSL_PATH("test/test.txt")) == true);
    NSL_ASSERT(nsl_path_eq(NSL_PATH("test"), NSL_PATH("test/test.txt")) == false);
}

static void test_path_join(void) {
    nsl_Arena arena = {0};

    // Basic join
    {
        nsl_Path parts[] = { NSL_PATH("foo"), NSL_PATH("bar") };
        nsl_Path result = nsl_path_join(2, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("foo/bar")));
    }

    // Trailing and leading slashes removed
    {
        nsl_Path parts[] = { NSL_PATH("foo/"), NSL_PATH("/bar/") };
        nsl_Path result = nsl_path_join(2, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("foo/bar/")));
    }

    // Leading slash in first part is preserved
    {
        nsl_Path parts[] = { NSL_PATH("/foo/"), NSL_PATH("/bar/") };
        nsl_Path result = nsl_path_join(2, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("/foo/bar/")));
    }

    // Empty parts skipped
    {
        nsl_Path parts[] = { NSL_PATH(""), NSL_PATH("foo"), NSL_PATH(""), NSL_PATH("bar") };
        nsl_Path result = nsl_path_join(4, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("foo/bar")));
    }

    // Single part
    {
        nsl_Path parts[] = { NSL_PATH("foo/") };
        nsl_Path result = nsl_path_join(1, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("foo/")));
    }

    // windows
    {
        nsl_Path parts[] = { NSL_PATH("c:/") };
        nsl_Path result = nsl_path_join(1, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("c:/")));
    }


    {
        nsl_Path parts[] = { NSL_PATH("/"), NSL_PATH("foo/"), NSL_PATH("//bar") };
        nsl_Path result = nsl_path_join(3, parts, &arena);
        NSL_ASSERT(nsl_str_eq(result, NSL_PATH("/foo/bar")));
    }


    nsl_arena_free(&arena);
}

static void test_nc_path_normalize(void) {
    nsl_Arena arena = {0};

    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("/usr/../test.txt"), &arena), NSL_PATH("/test.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("/../../text.txt"), &arena), NSL_PATH("/text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("../text.txt"), &arena), NSL_PATH("../text.txt")));

    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("c:\\..\\text.txt"), &arena), NSL_PATH("c:/text.txt")));

    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/test/../file.txt"), &arena), NSL_PATH("C:/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/test/./file.txt"), &arena), NSL_PATH("C:/test/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/test/.."), &arena), NSL_PATH("C:/")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/test/../"), &arena), NSL_PATH("C:/")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("./text.txt"), &arena), NSL_PATH("text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("../text.txt"), &arena), NSL_PATH("../text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("test/../text.txt"), &arena), NSL_PATH("text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("test//text.txt"), &arena), NSL_PATH("test/text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("/test//text.txt"), &arena), NSL_PATH("/test/text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:\\user\\docs\\..\\..\\text.txt"), &arena), NSL_PATH("C:/text.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:\\user\\..\\docs\\file.txt"), &arena), NSL_PATH("C:/docs/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:\\a\\b\\c\\..\\..\\..\\file.txt"), &arena), NSL_PATH("C:/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:\\a\\b\\c\\..\\..\\..\\..\\file.txt"), &arena), NSL_PATH("C:/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/a/b/c/../../../file.txt"), &arena), NSL_PATH("C:/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/a/b/c/../../../../file.txt"), &arena), NSL_PATH("C:/file.txt")));
    NSL_ASSERT(nsl_str_eq(nsl_path_normalize(NSL_PATH("C:/a/./b/./c/./file.txt"), &arena), NSL_PATH("C:/a/b/c/file.txt")));

    nsl_arena_free(&arena);
}

static void test_nc_path_parent(void) {
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("C:\\test.txt")), NSL_STR("C:\\")));

    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("/usr/bin")), NSL_PATH("/usr/")));

    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("/usr/bin/")), NSL_PATH("/usr/")));


    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("/usr")), NSL_PATH("/")));
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("/")), NSL_PATH("/")));

    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("foo/bar")), NSL_PATH("foo/")));
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("foo/bar/")), NSL_PATH("foo/")));
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("foo")), NSL_PATH(".")));
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH(".")), NSL_PATH(".")));
    NSL_ASSERT(nsl_str_eq(nsl_path_parent(NSL_PATH("")), NSL_PATH(".")));
}

void run_test_path(void);
void run_test_path(void) {
    test_nc_path_eq();
    test_path_join();
    test_nc_path_normalize();
    test_nc_path_parent();
}
