#ifndef CRYPTEX_API_CLIENT_H
#define CRYPTEX_API_CLIENT_H

#include "cryptex.h"

CryptexStatus api_client_init(void);
void api_client_cleanup(void);

/**
 * Perform HTTP GET request to CoinPaprika API endpoint.
 * Caller takes ownership of the returned string and MUST call free() on it.
 * Returns NULL on network failure or HTTP error.
 */
char *api_get(const char *endpoint);

#endif /* CRYPTEX_API_CLIENT_H */
