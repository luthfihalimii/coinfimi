#include <stdio.h>
#include <assert.h>
#include "cryptex.h"
#include "utils/validator.h"

static void test_validate_amount(void) {
    assert(validate_amount(10.5) == 1);
    assert(validate_amount(0.00000001) == 1);
    assert(validate_amount(0.0) == 0);
    assert(validate_amount(-1.0) == 0);
}

static void test_validate_symbol(void) {
    assert(validate_symbol("BTC") == 1);
    assert(validate_symbol("eth") == 1);
    assert(validate_symbol("") == 0);
    assert(validate_symbol(NULL) == 0);
    assert(validate_symbol("VERYLONGSYMBOLNAMETHATEXCEEDS") == 0);
}

int main(void) {
    printf("[TEST] Running test_validator...\n");
    test_validate_amount();
    test_validate_symbol();
    printf("[TEST] test_validator PASSED.\n");
    return 0;
}
