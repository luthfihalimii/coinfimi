#include "market.h"
#include "api/api_client.h"
#include "utils/json_parser.h"
#include <stdio.h>
#include <stdlib.h>
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
static Coin s_cache[sizeof(s_registry) / sizeof(s_registry[0])];
static bool s_cache_valid[sizeof(s_registry) / sizeof(s_registry[0])];
static double s_usd_per_idr;
static double s_usd_per_eur;
static bool s_idr_valid;
static bool s_eur_valid;
static char s_idr_updated[MAX_TIMESTAMP_LEN];
static char s_eur_updated[MAX_TIMESTAMP_LEN];

static int fetch_coin(size_t index) {
    char endpoint[96];
    int written = snprintf(endpoint, sizeof(endpoint),
                           "/tickers/%s?quotes=USD,IDR,EUR", s_registry[index].id);
    if (written < 0 || (size_t)written >= sizeof(endpoint)) {
        return -1;
    }

    char *body = api_get(endpoint);
    if (!body) {
        return -1;
    }

    Coin coin;
    int parse_status = parse_coin_data(body, "USD", &coin);
    free(body);

    if (parse_status != 0) {
        fprintf(stderr, "Failed to parse market data for %s\n", s_registry[index].symbol);
        return -1;
    }

    s_cache[index] = coin;
    s_cache_valid[index] = true;

    if (coin.price_usd > 0.0) {
        if (coin.price_idr > 0.0) {
            s_usd_per_idr = coin.price_usd / coin.price_idr;
            s_idr_valid = true;
            snprintf(s_idr_updated, sizeof(s_idr_updated), "%s", coin.last_updated);
        }
        if (coin.price_eur > 0.0) {
            s_usd_per_eur = coin.price_usd / coin.price_eur;
            s_eur_valid = true;
            snprintf(s_eur_updated, sizeof(s_eur_updated), "%s", coin.last_updated);
        }
    }

    return 0;
}

int market_init(void) {
    memset(s_cache, 0, sizeof(s_cache));
    memset(s_cache_valid, 0, sizeof(s_cache_valid));
    s_usd_per_idr = 0.0;
    s_usd_per_eur = 0.0;
    s_idr_valid = false;
    s_eur_valid = false;
    s_idr_updated[0] = '\0';
    s_eur_updated[0] = '\0';
    return 0;
}

const CoinInfo *market_get_registry(size_t *out_count) {
    if (out_count) {
        *out_count = s_registry_count;
    }
    return s_registry;
}

int market_find_coin(const char *symbol) {
    if (!symbol) {
        return -1;
    }

    for (size_t i = 0; i < s_registry_count; i++) {
        if (strcasecmp(s_registry[i].symbol, symbol) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int market_fetch_prices(void) {
    int failures = 0;

    for (size_t i = 0; i < s_registry_count; i++) {
        if (fetch_coin(i) != 0) {
            failures++;
        }
    }

    return (failures == 0) ? 0 : -1;
}

void market_display_prices(void) {
    const char *separator = "-------------------------------------------------------------------------------";

    printf("%s\n", separator);
    printf(" %-6s %-10s %14s %9s %14s %-20s\n",
           "SYMBOL", "NAME", "PRICE USD", "24H", "MARKET CAP", "UPDATED");
    printf("%s\n", separator);

    for (size_t i = 0; i < s_registry_count; i++) {
        if (!s_cache_valid[i]) {
            printf(" %-6s %-10s %14s %9s %14s %-20s\n",
                   s_registry[i].symbol, s_registry[i].name,
                   "N/A", "N/A", "N/A", "N/A");
            continue;
        }

        const Coin *coin = &s_cache[i];
        char change[16];
        char market_cap[32];

        snprintf(change, sizeof(change), "%+.2f%%", coin->percent_change_24h);
        if (coin->market_cap > 0.0) {
            snprintf(market_cap, sizeof(market_cap), "$%.0f", coin->market_cap);
        } else {
            snprintf(market_cap, sizeof(market_cap), "N/A");
        }

        printf(" %-6s %-10s %14.2f %9s %14s %-20s\n",
               coin->symbol, coin->name, coin->price_usd,
               change, market_cap, coin->last_updated);
    }

    printf("%s\n", separator);
    printf(" Source: CoinPaprika | Prices are informational, not trading prices.\n");
}

int market_refresh(void) {
    memset(s_cache_valid, 0, sizeof(s_cache_valid));
    s_usd_per_idr = 0.0;
    s_usd_per_eur = 0.0;
    s_idr_valid = false;
    s_eur_valid = false;
    s_idr_updated[0] = '\0';
    s_eur_updated[0] = '\0';
    return market_fetch_prices();
}

const Coin *market_get_coin_by_symbol(const char *symbol) {
    int index = market_find_coin(symbol);
    if (index < 0) {
        return NULL;
    }

    if (!s_cache_valid[index] && fetch_coin((size_t)index) != 0) {
        return NULL;
    }

    return &s_cache[index];
}

int market_get_usd_price(const char *symbol, double *out_price) {
    if (!symbol || !out_price) {
        return -1;
    }

    if (strcasecmp(symbol, "USD") == 0) {
        *out_price = 1.0;
        return 0;
    }

    if (strcasecmp(symbol, "IDR") == 0) {
        if (!s_idr_valid) {
            (void)market_get_coin_by_symbol("BTC");
        }
        if (!s_idr_valid || s_usd_per_idr <= 0.0) {
            return -1;
        }
        *out_price = s_usd_per_idr;
        return 0;
    }

    if (strcasecmp(symbol, "EUR") == 0) {
        if (!s_eur_valid) {
            (void)market_get_coin_by_symbol("BTC");
        }
        if (!s_eur_valid || s_usd_per_eur <= 0.0) {
            return -1;
        }
        *out_price = s_usd_per_eur;
        return 0;
    }

    const Coin *coin = market_get_coin_by_symbol(symbol);
    if (!coin) {
        return -1;
    }

    *out_price = coin->price_usd;
    return 0;
}

const char *market_get_fiat_updated(const char *symbol) {
    if (!symbol) {
        return NULL;
    }

    if (strcasecmp(symbol, "IDR") == 0) {
        return s_idr_updated;
    }
    if (strcasecmp(symbol, "EUR") == 0) {
        return s_eur_updated;
    }
    return NULL;
}
