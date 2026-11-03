#include "menu.h"
#include <stdio.h>
#include <stdlib.h>

void display_dashboard(void) {
    printf("+------------------------------------------------------+\n");
    printf("|     CRYPTO CURRENCY CONVERTER - CRYPTEX v%s      |\n", CRYPTEX_VERSION);
    printf("+------------------------------------------------------+\n");
    printf("| API STATUS : INITIALIZED                             |\n");
    printf("| DATA SOURCE: COINPAPRIKA                             |\n");
    printf("+------------------------------------------------------+\n");
    printf("|                                                      |\n");
    printf("|  MAIN MENU                                           |\n");
    printf("|                                                      |\n");
    printf("|  1. Currency Converter                               |\n");
    printf("|  2. Cryptocurrency List                              |\n");
    printf("|  3. Market Price                                     |\n");
    printf("|  4. Conversion History                               |\n");
    printf("|  5. Refresh Market Data                              |\n");
    printf("|  0. Exit                                             |\n");
    printf("|                                                      |\n");
    printf("+------------------------------------------------------+\n");
    printf("|  %s                                |\n", COINPAPRIKA_ATTRIBUTION);
    printf("+------------------------------------------------------+\n");
}

void display_main_menu(void) {
    display_dashboard();
}

void display_converter_menu(void) {
    printf("\n=== Currency Converter ===\n");
}

void display_market_menu(void) {
    printf("\n=== Market Price ===\n");
}

void display_history_menu(void) {
    printf("\n=== Conversion History ===\n");
}

void main_menu(void) {
    display_dashboard();
    printf("\n[Phase 1] Foundation initialized successfully.\n");
}
