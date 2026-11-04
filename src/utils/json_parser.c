#include "json_parser.h"
#include <math.h>
#include <string.h>
#include <cjson/cJSON.h>

static int copy_field(char *dest, size_t dest_size, const char *src) {
    size_t len = strlen(src);
    if (len >= dest_size) {
        return -1;
    }
    memcpy(dest, src, len + 1);
    return 0;
}

static double quote_price(const cJSON *quotes, const char *currency) {
    const cJSON *quote = cJSON_GetObjectItemCaseSensitive(quotes, currency);
    if (!cJSON_IsObject(quote)) {
        return 0.0;
    }

    const cJSON *price = cJSON_GetObjectItemCaseSensitive(quote, "price");
    if (!cJSON_IsNumber(price) || !isfinite(price->valuedouble) ||
        price->valuedouble <= 0.0) {
        return 0.0;
    }

    return price->valuedouble;
}

int parse_coin_data(const char *json_str, const char *currency, Coin *coin) {
    if (!json_str || !currency || !coin) {
        return -1;
    }

    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        return -1;
    }

    int rc = -1;
    memset(coin, 0, sizeof(*coin));

    const cJSON *id = cJSON_GetObjectItemCaseSensitive(root, "id");
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
    const cJSON *symbol = cJSON_GetObjectItemCaseSensitive(root, "symbol");
    const cJSON *quotes = cJSON_GetObjectItemCaseSensitive(root, "quotes");

    if (!cJSON_IsString(id) || !cJSON_IsString(name) || !cJSON_IsString(symbol) ||
        !cJSON_IsObject(quotes)) {
        goto done;
    }

    const cJSON *quote = cJSON_GetObjectItemCaseSensitive(quotes, currency);
    if (!cJSON_IsObject(quote)) {
        goto done;
    }

    const cJSON *price = cJSON_GetObjectItemCaseSensitive(quote, "price");
    if (!cJSON_IsNumber(price) || !isfinite(price->valuedouble) ||
        price->valuedouble <= 0.0) {
        goto done;
    }

    if (copy_field(coin->id, sizeof(coin->id), id->valuestring) != 0 ||
        copy_field(coin->name, sizeof(coin->name), name->valuestring) != 0 ||
        copy_field(coin->symbol, sizeof(coin->symbol), symbol->valuestring) != 0) {
        goto done;
    }

    coin->price_usd = price->valuedouble;
    coin->price_idr = quote_price(quotes, "IDR");
    coin->price_eur = quote_price(quotes, "EUR");

    const cJSON *change = cJSON_GetObjectItemCaseSensitive(quote, "percent_change_24h");
    if (cJSON_IsNumber(change) && isfinite(change->valuedouble)) {
        coin->percent_change_24h = change->valuedouble;
    }

    const cJSON *market_cap = cJSON_GetObjectItemCaseSensitive(quote, "market_cap");
    if (cJSON_IsNumber(market_cap) && isfinite(market_cap->valuedouble) &&
        market_cap->valuedouble >= 0.0) {
        coin->market_cap = market_cap->valuedouble;
    }

    const cJSON *updated = cJSON_GetObjectItemCaseSensitive(root, "last_updated");
    if (cJSON_IsString(updated)) {
        if (copy_field(coin->last_updated, sizeof(coin->last_updated), updated->valuestring) != 0) {
            goto done;
        }
    }

    rc = 0;

done:
    cJSON_Delete(root);
    return rc;
}
