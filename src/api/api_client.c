#include "api_client.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define API_CONNECT_TIMEOUT_SECONDS 10L
#define API_TOTAL_TIMEOUT_SECONDS 15L
#define API_MAX_RESPONSE_BYTES (1024 * 1024)

typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    ResponseBuffer *buffer = userdata;

    if (size != 0 && nmemb > SIZE_MAX / size) {
        return 0;
    }

    size_t chunk = size * nmemb;
    if (chunk > API_MAX_RESPONSE_BYTES - buffer->size) {
        return 0;
    }

    char *data = realloc(buffer->data, buffer->size + chunk + 1);
    if (!data) {
        return 0;
    }

    buffer->data = data;
    memcpy(buffer->data + buffer->size, ptr, chunk);
    buffer->size += chunk;
    buffer->data[buffer->size] = '\0';
    return chunk;
}

static char *build_url(const char *endpoint) {
    const char *separator = (endpoint[0] == '/') ? "" : "/";
    size_t length = strlen(COINPAPRIKA_BASE_URL) + strlen(separator) +
                    strlen(endpoint) + 1;

    char *url = malloc(length);
    if (!url) {
        return NULL;
    }

    snprintf(url, length, "%s%s%s", COINPAPRIKA_BASE_URL, separator, endpoint);
    return url;
}

static const char *status_message(CryptexStatus status) {
    switch (status) {
        case CRYPTEX_ERR_HTTP_TIMEOUT:
            return "connection timed out";
        case CRYPTEX_ERR_HTTP_404:
            return "cryptocurrency data not found (HTTP 404)";
        case CRYPTEX_ERR_HTTP_429:
            return "API rate limit reached (HTTP 429)";
        case CRYPTEX_ERR_HTTP_5XX:
            return "CoinPaprika server error (HTTP 5xx)";
        case CRYPTEX_ERR_HTTP:
            return "network or HTTP error";
        case CRYPTEX_ERR_INVALID_PARAM:
            return "invalid API endpoint";
        default:
            return "API request failed";
    }
}

CryptexStatus api_client_init(void) {
    CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    return (code == CURLE_OK) ? CRYPTEX_OK : CRYPTEX_ERR_HTTP;
}

void api_client_cleanup(void) {
    curl_global_cleanup();
}

char *api_get(const char *endpoint) {
    if (!endpoint || endpoint[0] == '\0') {
        fprintf(stderr, "API error: %s\n", status_message(CRYPTEX_ERR_INVALID_PARAM));
        return NULL;
    }

    char *url = build_url(endpoint);
    if (!url) {
        fprintf(stderr, "API error: out of memory\n");
        return NULL;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "API error: failed to initialize HTTP client\n");
        free(url);
        return NULL;
    }

    ResponseBuffer buffer = {0};
    char error_buffer[CURL_ERROR_SIZE] = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, API_CONNECT_TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, API_TOTAL_TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "cryptex/" CRYPTEX_VERSION);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);

    CURLcode result = curl_easy_perform(curl);
    long http_code = 0;
    if (result == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    }

    CryptexStatus status = CRYPTEX_OK;
    if (result == CURLE_OPERATION_TIMEDOUT) {
        status = CRYPTEX_ERR_HTTP_TIMEOUT;
    } else if (result != CURLE_OK) {
        status = CRYPTEX_ERR_HTTP;
    } else if (http_code == 404) {
        status = CRYPTEX_ERR_HTTP_404;
    } else if (http_code == 429) {
        status = CRYPTEX_ERR_HTTP_429;
    } else if (http_code >= 500 && http_code <= 599) {
        status = CRYPTEX_ERR_HTTP_5XX;
    } else if (http_code != 200) {
        status = CRYPTEX_ERR_HTTP;
    }

    curl_easy_cleanup(curl);
    free(url);

    if (status != CRYPTEX_OK) {
        if (result != CURLE_OK && error_buffer[0] != '\0') {
            fprintf(stderr, "API error: %s (%s)\n", status_message(status), error_buffer);
        } else {
            fprintf(stderr, "API error: %s\n", status_message(status));
        }
        free(buffer.data);
        return NULL;
    }

    if (!buffer.data) {
        buffer.data = malloc(1);
        if (!buffer.data) {
            return NULL;
        }
        buffer.data[0] = '\0';
    }

    return buffer.data;
}
