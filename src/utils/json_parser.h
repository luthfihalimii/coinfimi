#ifndef CRYPTEX_JSON_PARSER_H
#define CRYPTEX_JSON_PARSER_H

#include "cryptex.h"

/**
 * Parses coin JSON data from CoinPaprika ticker endpoint.
 * Returns 0 on success, non-zero on failure.
 */
int parse_coin_data(const char *json_str, const char *currency, Coin *coin);

#endif /* CRYPTEX_JSON_PARSER_H */
