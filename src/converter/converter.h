#ifndef CRYPTEX_CONVERTER_H
#define CRYPTEX_CONVERTER_H

#include "cryptex.h"

/**
 * Basic conversion function matching ARCHITECTURE.md spec.
 * Returns -1.0 on invalid input.
 */
double convert_currency(double amount, double from_price, double to_price);

/**
 * Robust status-based conversion calculation.
 */
CryptexStatus convert_calculate(double amount, double from_price, double to_price, double *out_result, double *out_rate);

#endif /* CRYPTEX_CONVERTER_H */
