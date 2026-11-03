#ifndef CRYPTEX_H
#define CRYPTEX_H

#include <stddef.h>
#include <stdbool.h>

#define CRYPTEX_VERSION "1.0.0"
#define CRYPTEX_APP_NAME "CRYPTEX"
#define COINPAPRIKA_BASE_URL "https://api.coinpaprika.com/v1"
#define COINPAPRIKA_ATTRIBUTION "Powered by CoinPaprika"
#define CRYPTEX_HISTORY_FILE "data/history.csv"

#define MAX_ID_LEN 64
#define MAX_NAME_LEN 64
#define MAX_SYMBOL_LEN 16
#define MAX_TIMESTAMP_LEN 32

/* Status / Return Codes */
typedef enum {
    CRYPTEX_OK = 0,
    CRYPTEX_ERR_GENERIC = -1,
    CRYPTEX_ERR_INVALID_PARAM = -2,
    CRYPTEX_ERR_HTTP = -3,
    CRYPTEX_ERR_HTTP_TIMEOUT = -4,
    CRYPTEX_ERR_HTTP_404 = -5,
    CRYPTEX_ERR_HTTP_429 = -6,
    CRYPTEX_ERR_HTTP_5XX = -7,
    CRYPTEX_ERR_JSON = -8,
    CRYPTEX_ERR_NOT_FOUND = -9,
    CRYPTEX_ERR_FILE_IO = -10,
    CRYPTEX_ERR_CALCULATION = -11
} CryptexStatus;

/* Core Data Models */
typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    char symbol[MAX_SYMBOL_LEN];
    double price_usd;
    double percent_change_24h;
    char last_updated[MAX_TIMESTAMP_LEN];
} Coin;

typedef struct {
    char from_symbol[MAX_SYMBOL_LEN];
    char to_symbol[MAX_SYMBOL_LEN];
    double amount;
    double exchange_rate;
    double result;
    char timestamp[MAX_TIMESTAMP_LEN];
} Conversion;

typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    char symbol[MAX_SYMBOL_LEN];
} CoinInfo;

typedef struct {
    Coin coin;
    bool is_valid;
} PriceCache;

/* Application Lifecycle */
CryptexStatus app_initialize(void);
void app_cleanup(void);

#endif /* CRYPTEX_H */
