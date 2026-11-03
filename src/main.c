#include <stdio.h>
#include <stdlib.h>
#include "cryptex.h"
#include "api/api_client.h"
#include "history/history.h"
#include "market/market.h"
#include "ui/menu.h"

CryptexStatus app_initialize(void) {
    CryptexStatus status = api_client_init();
    if (status != CRYPTEX_OK) {
        fprintf(stderr, "Failed to initialize API client\n");
        return status;
    }

    if (history_initialize() != 0) {
        fprintf(stderr, "Warning: Failed to initialize history storage\n");
    }

    market_init();
    return CRYPTEX_OK;
}

void app_cleanup(void) {
    api_client_cleanup();
}

int main(void) {
    if (app_initialize() != CRYPTEX_OK) {
        return EXIT_FAILURE;
    }

    main_menu();

    app_cleanup();
    return EXIT_SUCCESS;
}
