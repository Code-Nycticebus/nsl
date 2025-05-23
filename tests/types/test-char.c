#include "nc/types/char.h"

static void test_char_test(void) {
    NC_ASSERT(nc_char_is_alnum('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_alnum('A') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_alnum('1') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_alnum(' ') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_alpha('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_alpha('A') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_alpha('1') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_lower('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_lower('A') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_upper('A') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_upper('a') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_space(' ') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_space('a') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_cntrl('\0') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_cntrl('\n') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_cntrl('a') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_print('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_print(' ') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_print('\0') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_graph('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_graph(' ') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_blank(' ') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_blank('\t') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_blank('\n') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_punct('.') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_punct('[') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_punct(']') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_punct('a') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_digit('1') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_digit('a') == false && "Did not test correctly");

    NC_ASSERT(nc_char_is_xdigit('1') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_xdigit('a') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_xdigit('A') == true && "Did not test correctly");
    NC_ASSERT(nc_char_is_xdigit('z') == false && "Did not test correctly");
}

static void test_char_convertion(void) {
    NC_ASSERT(nc_char_to_upper('a') == 'A' && "should be 'A'");
    NC_ASSERT(nc_char_to_lower('A') == 'a' && "should be 'a'");

    NC_ASSERT(nc_char_to_u8('2') == 2 && "should be 2");
    NC_ASSERT(nc_char_from_u8(2) == '2' && "should be '2'");

    NC_ASSERT(nc_char_hex_to_u8('2') == 2 && "should be 2");
    NC_ASSERT(nc_char_hex_to_u8('a') == 10 && "should be 10");
    NC_ASSERT(nc_char_hex_to_u8('A') == 10 && "should be 10");

    NC_ASSERT(nc_char_hex_from_u8(2) == '2' && "should be '2'");
    NC_ASSERT(nc_char_hex_from_u8(10) == 'a' && "should be 'a'");
    NC_ASSERT(nc_char_HEX_from_u8(10) == 'A' && "should be 'A'");
}

void run_test_char(void) {
    test_char_test();
    test_char_convertion();
}
