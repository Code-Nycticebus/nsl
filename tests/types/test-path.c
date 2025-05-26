#include "nc/types/path.h"

#include "nc/types/str.h"
#include <stdio.h>

static void test_nc_path_eq(void) {
    NC_ASSERT(nc_path_eq(NC_PATH("test/test.txt"), NC_PATH("test/test.txt")) == true);
    NC_ASSERT(nc_path_eq(NC_PATH("test"), NC_PATH("test/test.txt")) == false);
}

static void test_path_join(void) {
    nc_Arena arena = {0};

    // Basic join
    {
        nc_Path parts[] = { NC_PATH("foo"), NC_PATH("bar") };
        nc_Path result = nc_path_join(2, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo/bar")));
    }

    // Trailing and leading slashes removed
    {
        nc_Path parts[] = { NC_PATH("foo/"), NC_PATH("/bar/") };
        nc_Path result = nc_path_join(2, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo/bar/")));
    }

    // Leading slash in first part is preserved
    {
        nc_Path parts[] = { NC_PATH("/foo/"), NC_PATH("/bar/") };
        nc_Path result = nc_path_join(2, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("/foo/bar/")));
    }

    // Empty parts skipped
    {
        nc_Path parts[] = { NC_PATH(""), NC_PATH("foo"), NC_PATH(""), NC_PATH("bar") };
        nc_Path result = nc_path_join(4, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo/bar")));
    }

    // Single part
    {
        nc_Path parts[] = { NC_PATH("foo/") };
        nc_Path result = nc_path_join(1, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo/")));
    }

    // windows
    {
        nc_Path parts[] = { NC_PATH("c:/") };
        nc_Path result = nc_path_join(1, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("c:/")));
    }


    {
        nc_Path parts[] = { NC_PATH("/"), NC_PATH("foo/"), NC_PATH("//bar") };
        nc_Path result = nc_path_join(3, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("/foo/bar")));
    }


    nc_arena_free(&arena);
}

static void test_nc_path_normalize(void) {
    nc_Arena arena = {0};

    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/usr/../test.txt"), &arena), NC_PATH("/test.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/../../text.txt"), &arena), NC_PATH("/text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("../text.txt"), &arena), NC_PATH("../text.txt")));

    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("c:\\..\\text.txt"), &arena), NC_PATH("c:/text.txt")));

    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/test/../file.txt"), &arena), NC_PATH("C:/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/test/./file.txt"), &arena), NC_PATH("C:/test/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/test/.."), &arena), NC_PATH("C:/")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/test/../"), &arena), NC_PATH("C:/")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("./text.txt"), &arena), NC_PATH("text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("../text.txt"), &arena), NC_PATH("../text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("test/../text.txt"), &arena), NC_PATH("text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("test//text.txt"), &arena), NC_PATH("test/text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/test//text.txt"), &arena), NC_PATH("/test/text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:\\user\\docs\\..\\..\\text.txt"), &arena), NC_PATH("C:/text.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:\\user\\..\\docs\\file.txt"), &arena), NC_PATH("C:/docs/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:\\a\\b\\c\\..\\..\\..\\file.txt"), &arena), NC_PATH("C:/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:\\a\\b\\c\\..\\..\\..\\..\\file.txt"), &arena), NC_PATH("C:/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/a/b/c/../../../file.txt"), &arena), NC_PATH("C:/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/a/b/c/../../../../file.txt"), &arena), NC_PATH("C:/file.txt")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("C:/a/./b/./c/./file.txt"), &arena), NC_PATH("C:/a/b/c/file.txt")));

    nc_arena_free(&arena);
}

static void test_nc_path_parent(void) {
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("C:\\test.txt")), NC_STR("C:\\")));

    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr/bin")), NC_PATH("/usr/")));

    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr/bin/")), NC_PATH("/usr/")));


    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr")), NC_PATH("/")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/")), NC_PATH("/")));

    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo/bar")), NC_PATH("foo/")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo/bar/")), NC_PATH("foo/")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo")), NC_PATH(".")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH(".")), NC_PATH(".")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("")), NC_PATH(".")));
}

void run_test_path(void);
void run_test_path(void) {
    test_nc_path_eq();
    test_path_join();
    test_nc_path_normalize();
    test_nc_path_parent();
}
