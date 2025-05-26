#include "nc/types/path.h"

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
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo/bar")));
    }

    // Leading slash in first part is preserved
    {
        nc_Path parts[] = { NC_PATH("/foo/"), NC_PATH("/bar/") };
        nc_Path result = nc_path_join(2, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("/foo/bar")));
    }

    // Empty parts skipped
    {
        nc_Path parts[] = { NC_PATH(""), NC_PATH("foo"), NC_PATH(""), NC_PATH("bar") };
        nc_Path result = nc_path_join(4, parts, &arena);
        // NC_ASSERT(nc_str_eq(result, NC_PATH("foo/bar")));
    }

    // Single part
    {
        nc_Path parts[] = { NC_PATH("foo") };
        nc_Path result = nc_path_join(1, parts, &arena);
        NC_ASSERT(nc_str_eq(result, NC_PATH("foo")));
    }

    nc_arena_free(&arena);
}

static void test_path_normalize(void) {
    nc_Arena arena = {0};

    // Absolute path normalization
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/a/b/../c"), &arena), NC_PATH("/a/c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/a/b/../../c"), &arena), NC_PATH("/c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/a/b/../../../c"), &arena), NC_PATH("/c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/a/./b/../c/"), &arena), NC_PATH("/a/c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/usr/../../../usr/../../test.txt"), &arena), NC_PATH("/test.txt")));

    // Relative path normalization
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("a/b/../c"), &arena), NC_PATH("a/c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("a/b/../../c"), &arena), NC_PATH("c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("a/b/../../../c"), &arena), NC_PATH("../c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("../../c"), &arena), NC_PATH("../../c")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("a/./b/../c/"), &arena), NC_PATH("a/c")));

    // Dot-only path
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("."), &arena), NC_PATH("")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("././."), &arena), NC_PATH("")));

    // Parent-only path
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH(".."), &arena), NC_PATH("..")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("../.."), &arena), NC_PATH("../..")));
    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("../../a"), &arena), NC_PATH("../../a")));

    NC_ASSERT(nc_str_eq(nc_path_normalize(NC_PATH("/"), &arena), NC_PATH("/")));

    nc_arena_free(&arena);
}

static void test_nc_path_parent(void) {
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr/bin")), NC_PATH("/usr")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr/bin/")), NC_PATH("/usr")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/usr")), NC_PATH("")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("/")), NC_PATH("")));

    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo/bar")), NC_PATH("foo")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo/bar/")), NC_PATH("foo")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("foo")), NC_PATH("")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH(".")), NC_PATH("")));
    NC_ASSERT(nc_str_eq(nc_path_parent(NC_PATH("")), NC_PATH("")));
}

void run_test_path(void) {
    test_nc_path_eq();
    test_path_join();
    test_path_normalize();
    test_nc_path_parent();
}
