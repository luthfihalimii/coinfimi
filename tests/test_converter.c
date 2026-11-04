#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "cryptex.h"
#include "converter/converter.h"

#define EPSILON 0.0001

static void test_convert_currency_basic(void) {
    /* 0.5 BTC at $95,000 to ETH at $3,200 -> 14.84375 ETH */
    double result = convert_currency(0.5, 95000.0, 3200.0);
    assert(fabs(result - 14.84375) < EPSILON);

    /* 0.5 BTC at $95,000 to USD ($1 per USD) -> $47,500 */
    result = convert_currency(0.5, 95000.0, 1.0);
    assert(fabs(result - 47500.0) < EPSILON);

    /* $1,000 to BTC at $95,000 -> 1000 / 95000 = 0.0105263 */
    result = convert_currency(1000.0, 1.0, 95000.0);
    assert(fabs(result - 0.0105263) < EPSILON);
}

static void test_convert_currency_invalid(void) {
    /* Zero or negative amount */
    assert(convert_currency(0.0, 95000.0, 3200.0) == -1.0);
    assert(convert_currency(-5.0, 95000.0, 3200.0) == -1.0);

    /* Zero or negative price */
    assert(convert_currency(1.0, 0.0, 3200.0) == -1.0);
    assert(convert_currency(1.0, 95000.0, 0.0) == -1.0);
    assert(convert_currency(1.0, -100.0, 3200.0) == -1.0);
}

static void test_convert_rejects_non_finite(void) {
    assert(convert_currency(INFINITY, 95000.0, 3200.0) == -1.0);
    assert(convert_currency(1.0, INFINITY, 3200.0) == -1.0);
    assert(convert_currency(1.0, 95000.0, INFINITY) == -1.0);
    assert(convert_currency(NAN, 95000.0, 3200.0) == -1.0);

    double result = 0.0;
    double rate = 0.0;
    assert(convert_calculate(INFINITY, 95000.0, 3200.0, &result, &rate) == CRYPTEX_ERR_INVALID_PARAM);
    assert(convert_calculate(1.0, NAN, 3200.0, &result, &rate) == CRYPTEX_ERR_INVALID_PARAM);
    assert(convert_calculate(1.0, 95000.0, INFINITY, &result, &rate) == CRYPTEX_ERR_INVALID_PARAM);
}

static void test_convert_calculate_status(void) {
    double result = 0.0;
    double rate = 0.0;

    CryptexStatus status = convert_calculate(0.5, 95000.0, 3200.0, &result, &rate);
    assert(status == CRYPTEX_OK);
    assert(fabs(rate - 29.6875) < EPSILON);
    assert(fabs(result - 14.84375) < EPSILON);

    /* Null pointer checks */
    assert(convert_calculate(1.0, 100.0, 50.0, NULL, &rate) == CRYPTEX_ERR_INVALID_PARAM);
    assert(convert_calculate(1.0, 100.0, 50.0, &result, NULL) == CRYPTEX_ERR_INVALID_PARAM);
}

int main(void) {
    printf("[TEST] Running test_converter...\n");
    test_convert_currency_basic();
    test_convert_currency_invalid();
    test_convert_rejects_non_finite();
    test_convert_calculate_status();
    printf("[TEST] test_converter PASSED.\n");
    return 0;
}
