#include "api_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

CryptexStatus api_client_init(void) {
    CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    return (code == CURLE_OK) ? CRYPTEX_OK : CRYPTEX_ERR_HTTP;
}

void api_client_cleanup(void) {
    curl_global_cleanup();
}

char *api_get(const char *endpoint) {
    (void)endpoint;
    /* Implemented in Phase 2 */
    return NULL;
}
