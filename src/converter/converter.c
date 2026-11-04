#include "converter.h"
#include <math.h>

static int is_valid_price(double price) {
    return isfinite(price) && price > 0.0;
}

double convert_currency(double amount, double from_price, double to_price) {
    if (!isfinite(amount) || amount <= 0.0 ||
        !is_valid_price(from_price) || !is_valid_price(to_price)) {
        return -1.0;
    }
    return amount * (from_price / to_price);
}

CryptexStatus convert_calculate(double amount, double from_price, double to_price, double *out_result, double *out_rate) {
    if (!isfinite(amount) || amount <= 0.0 ||
        !is_valid_price(from_price) || !is_valid_price(to_price)) {
        return CRYPTEX_ERR_INVALID_PARAM;
    }
    if (!out_result || !out_rate) {
        return CRYPTEX_ERR_INVALID_PARAM;
    }

    *out_rate = from_price / to_price;
    *out_result = amount * (*out_rate);
    return CRYPTEX_OK;
}
