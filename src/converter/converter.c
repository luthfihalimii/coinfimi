#include "converter.h"

double convert_currency(double amount, double from_price, double to_price) {
    if (amount <= 0.0 || from_price <= 0.0 || to_price <= 0.0) {
        return -1.0;
    }
    return amount * (from_price / to_price);
}

CryptexStatus convert_calculate(double amount, double from_price, double to_price, double *out_result, double *out_rate) {
    if (amount <= 0.0 || from_price <= 0.0 || to_price <= 0.0) {
        return CRYPTEX_ERR_INVALID_PARAM;
    }
    if (!out_result || !out_rate) {
        return CRYPTEX_ERR_INVALID_PARAM;
    }

    *out_rate = from_price / to_price;
    *out_result = amount * (*out_rate);
    return CRYPTEX_OK;
}
