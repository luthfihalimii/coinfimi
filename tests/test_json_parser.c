#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "cryptex.h"
#include "utils/json_parser.h"

static const char *VALID_JSON =
    "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
    "\"last_updated\":\"2026-11-04T04:00:16Z\","
    "\"quotes\":{\"USD\":{\"price\":76400.65773097219,"
    "\"percent_change_24h\":-0.55,\"market_cap\":1536243186271}}}";

static void test_parse_valid(void) {
    Coin coin;
    memset(&coin, 0, sizeof(coin));

    assert(parse_coin_data(VALID_JSON, "USD", &coin) == 0);
    assert(strcmp(coin.id, "btc-bitcoin") == 0);
    assert(strcmp(coin.name, "Bitcoin") == 0);
    assert(strcmp(coin.symbol, "BTC") == 0);
    assert(coin.price_usd > 0.0);
    assert(coin.percent_change_24h < 0.0);
    assert(coin.market_cap > 0.0);
    assert(strcmp(coin.last_updated, "2026-11-04T04:00:16Z") == 0);
}

static void test_parse_invalid_json(void) {
    Coin coin;
    assert(parse_coin_data("{not json", "USD", &coin) != 0);
}

static void test_parse_missing_quote(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{}}";
    assert(parse_coin_data(json, "USD", &coin) != 0);
}

static void test_parse_missing_price(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{\"USD\":{}}}";
    assert(parse_coin_data(json, "USD", &coin) != 0);
}

static void test_parse_zero_price(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{\"USD\":{\"price\":0}}}";
    assert(parse_coin_data(json, "USD", &coin) != 0);
}

static void test_parse_rejects_non_finite_price(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{\"USD\":{\"price\":1e999}}}";
    assert(parse_coin_data(json, "USD", &coin) != 0);
}

static void test_parse_optional_fields(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{\"USD\":{\"price\":10}}}";

    memset(&coin, 0, sizeof(coin));
    assert(parse_coin_data(json, "USD", &coin) == 0);
    assert(coin.percent_change_24h == 0.0);
    assert(coin.market_cap == 0.0);
    assert(coin.price_idr == 0.0);
    assert(coin.price_eur == 0.0);
    assert(coin.last_updated[0] == '\0');
}

static void test_parse_multi_fiat_quotes(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\",\"symbol\":\"BTC\","
        "\"quotes\":{"
        "\"USD\":{\"price\":100},"
        "\"IDR\":{\"price\":1600000},"
        "\"EUR\":{\"price\":90}}}";

    memset(&coin, 0, sizeof(coin));
    assert(parse_coin_data(json, "USD", &coin) == 0);
    assert(coin.price_usd == 100.0);
    assert(coin.price_idr == 1600000.0);
    assert(coin.price_eur == 90.0);
}

static void test_parse_rejects_null(void) {
    Coin coin;
    assert(parse_coin_data(NULL, "USD", &coin) != 0);
    assert(parse_coin_data(VALID_JSON, NULL, &coin) != 0);
    assert(parse_coin_data(VALID_JSON, "USD", NULL) != 0);
}

static void test_parse_rejects_oversized_symbol(void) {
    Coin coin;
    const char *json =
        "{\"id\":\"btc-bitcoin\",\"name\":\"Bitcoin\","
        "\"symbol\":\"THIS-SYMBOL-IS-WAY-TOO-LONG\","
        "\"quotes\":{\"USD\":{\"price\":10}}}";
    assert(parse_coin_data(json, "USD", &coin) != 0);
}

int main(void) {
    printf("[TEST] Running test_json_parser...\n");
    test_parse_valid();
    test_parse_invalid_json();
    test_parse_missing_quote();
    test_parse_missing_price();
    test_parse_zero_price();
    test_parse_rejects_non_finite_price();
    test_parse_optional_fields();
    test_parse_multi_fiat_quotes();
    test_parse_rejects_null();
    test_parse_rejects_oversized_symbol();
    printf("[TEST] test_json_parser PASSED.\n");
    return 0;
}
