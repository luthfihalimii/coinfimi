#ifndef CRYPTEX_MARKET_H
#define CRYPTEX_MARKET_H

#include "cryptex.h"

int market_init(void);
int market_fetch_prices(void);
void market_display_prices(void);
int market_find_coin(const char *symbol);
void market_refresh(void);

const CoinInfo *market_get_registry(size_t *out_count);
const Coin *market_get_coin_by_symbol(const char *symbol);

#endif /* CRYPTEX_MARKET_H */
