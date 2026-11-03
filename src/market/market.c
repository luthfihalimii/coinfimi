#include "market.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

static const CoinInfo s_registry[] = {
    {"btc-bitcoin", "Bitcoin", "BTC"},
    {"eth-ethereum", "Ethereum", "ETH"},
    {"usdt-tether", "Tether", "USDT"},
    {"bnb-binance-coin", "BNB", "BNB"},
    {"sol-solana", "Solana", "SOL"},
    {"xrp-xrp", "XRP", "XRP"},
    {"doge-dogecoin", "Dogecoin", "DOGE"},
    {"ada-cardano", "Cardano", "ADA"}
};

static const size_t s_registry_count = sizeof(s_registry) / sizeof(s_registry[0]);

int market_init(void) {
    return 0;
}

const CoinInfo *market_get_registry(size_t *out_count) {
    if (out_count) {
        *out_count = s_registry_count;
    }
    return s_registry;
}

int market_find_coin(const char *symbol) {
    if (!symbol) return -1;
    for (size_t i = 0; i < s_registry_count; i++) {
        if (strcasecmp(s_registry[i].symbol, symbol) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int market_fetch_prices(void) {
    /* Implemented in Phase 2/3 */
    return 0;
}

void market_display_prices(void) {
    /* Implemented in Phase 4 */
    printf("[Market] Displaying prices placeholder...\n");
}

void market_refresh(void) {
    market_fetch_prices();
}

const Coin *market_get_coin_by_symbol(const char *symbol) {
    (void)symbol;
    return NULL;
}
