#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cryptex.h"
#include "api/api_client.h"
#include "converter/converter.h"
#include "market/market.h"

static void test_api_get_success(void) {
    char *body = api_get("/tickers/btc-bitcoin");
    assert(body != NULL);
    assert(strstr(body, "btc-bitcoin") != NULL);
    free(body);
}

static void test_api_get_not_found(void) {
    char *body = api_get("/tickers/not-a-real-coin-xyz");
    assert(body == NULL);
}

static void test_market_fiat_rates(void) {
    double usd_price = 0.0;
    double idr_price = 0.0;
    double eur_price = 0.0;

    assert(market_get_usd_price("USD", &usd_price) == 0);
    assert(fabs(usd_price - 1.0) < 1e-12);

    assert(market_get_usd_price("IDR", &idr_price) == 0);
    assert(idr_price > 0.0 && idr_price < 0.01);

    assert(market_get_usd_price("EUR", &eur_price) == 0);
    assert(eur_price > 0.5 && eur_price < 2.0);

    const char *idr_updated = market_get_fiat_updated("IDR");
    assert(idr_updated != NULL && idr_updated[0] != '\0');
    const char *eur_updated = market_get_fiat_updated("EUR");
    assert(eur_updated != NULL && eur_updated[0] != '\0');
    assert(market_get_fiat_updated("USD") == NULL);

    double result = 0.0;
    double rate = 0.0;
    assert(convert_calculate(1.0, 1.0, idr_price, &result, &rate) == CRYPTEX_OK);
    assert(result > 1000.0);

    assert(convert_calculate(100000.0, idr_price, 1.0, &result, &rate) == CRYPTEX_OK);
    assert(result > 0.0 && result < 100000.0);
}

static void test_api_get_connection_failure(void) {
    assert(setenv("https_proxy", "http://127.0.0.1:1", 1) == 0);
    assert(setenv("HTTPS_PROXY", "http://127.0.0.1:1", 1) == 0);
    assert(unsetenv("no_proxy") == 0);
    assert(unsetenv("NO_PROXY") == 0);

    char *body = api_get("/tickers/btc-bitcoin");
    assert(body == NULL);

    unsetenv("https_proxy");
    unsetenv("HTTPS_PROXY");
}

static void test_market_fetch_coin(void) {
    const Coin *coin = market_get_coin_by_symbol("BTC");
    assert(coin != NULL);
    assert(strcmp(coin->symbol, "BTC") == 0);
    assert(coin->price_usd > 0.0);
    assert(coin->last_updated[0] != '\0');
}

static void test_market_conversion(void) {
    const Coin *btc = market_get_coin_by_symbol("BTC");
    const Coin *eth = market_get_coin_by_symbol("ETH");
    assert(btc != NULL);
    assert(eth != NULL);

    double result = 0.0;
    double rate = 0.0;

    assert(convert_calculate(0.5, btc->price_usd, eth->price_usd, &result, &rate) == CRYPTEX_OK);
    assert(result > 0.0);
    assert(fabs(result - (0.5 * btc->price_usd / eth->price_usd)) < 1e-9);

    assert(convert_calculate(0.5, btc->price_usd, 1.0, &result, &rate) == CRYPTEX_OK);
    assert(fabs(result - (0.5 * btc->price_usd)) < 1e-9);
}

int main(void) {
    assert(api_client_init() == CRYPTEX_OK);

    test_api_get_success();
    test_api_get_not_found();
    test_market_fetch_coin();
    test_market_conversion();
    test_market_fiat_rates();
    test_api_get_connection_failure();

    api_client_cleanup();
    printf("[TEST] test_api_client PASSED.\n");
    return 0;
}
