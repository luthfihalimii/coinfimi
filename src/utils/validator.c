#include "validator.h"
#include <math.h>
#include <string.h>

int validate_amount(double amount) {
    return isfinite(amount) && amount > 0.0;
}

int validate_symbol(const char *symbol) {
    if (!symbol || strlen(symbol) == 0 || strlen(symbol) >= MAX_SYMBOL_LEN) {
        return 0;
    }
    return 1;
}
