#ifndef CRYPTEX_MARKET_H
#define CRYPTEX_MARKET_H

#include "cryptex.h"

int market_init(void);
int market_fetch_prices(void);
void market_display_prices(void);
int market_find_coin(const char *symbol);
int market_refresh(void);

const CoinInfo *market_get_registry(size_t *out_count);
const Coin *market_get_coin_by_symbol(const char *symbol);
int market_get_usd_price(const char *symbol, double *out_price);
const char *market_get_fiat_updated(const char *symbol);

#endif /* CRYPTEX_MARKET_H */
