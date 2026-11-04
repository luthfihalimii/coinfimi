#include "menu.h"
#include "converter/converter.h"
#include "history/history.h"
#include "market/market.h"
#include "utils/time_utils.h"
#include "utils/validator.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#define UI_CLEAR "\033[2J\033[H"
#define INPUT_SIZE 128
#define UI_WIDTH_SEPARATOR "------------------------------------------------------------------------------"
#define CONVERTER_SEPARATOR "------------------------------------------"

typedef enum {
    API_STATUS_READY,
    API_STATUS_CONNECTED,
    API_STATUS_ERROR
} ApiStatus;

static ApiStatus s_api_status = API_STATUS_READY;

static const char *api_status_text(void) {
    switch (s_api_status) {
        case API_STATUS_CONNECTED:
            return "CONNECTED";
        case API_STATUS_ERROR:
            return "ERROR";
        default:
            return "READY";
    }
}

static void ui_clear(void) {
    if (isatty(fileno(stdout))) {
        fputs(UI_CLEAR, stdout);
    }
}

static void trim_whitespace(char *text) {
    char *start = text;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != text) {
        memmove(text, start, strlen(start) + 1);
    }

    size_t length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
}

static int read_line(char *buffer, size_t size) {
    if (!fgets(buffer, (int)size, stdin)) {
        return -1;
    }

    size_t length = strlen(buffer);
    while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r')) {
        buffer[--length] = '\0';
    }

    trim_whitespace(buffer);
    return 0;
}

static int read_int(const char *prompt, int minimum, int maximum, int *out) {
    char buffer[INPUT_SIZE];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (read_line(buffer, sizeof(buffer)) != 0) {
            return -1;
        }

        char *end = NULL;
        long value = strtol(buffer, &end, 10);
        if (end != buffer && *end == '\0' && value >= minimum && value <= maximum) {
            *out = (int)value;
            return 0;
        }

        printf(" Invalid choice. Enter %d-%d.\n", minimum, maximum);
    }
}

static int read_amount(const char *prompt, double *out) {
    char buffer[INPUT_SIZE];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (read_line(buffer, sizeof(buffer)) != 0) {
            return -1;
        }

        char *end = NULL;
        double value = strtod(buffer, &end);
        if (end != buffer && *end == '\0' && validate_amount(value)) {
            *out = value;
            return 0;
        }

        printf(" Invalid amount. Enter a positive number.\n");
    }
}

static int read_symbol(const char *prompt, char *out, size_t size) {
    char buffer[INPUT_SIZE];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);
        if (read_line(buffer, sizeof(buffer)) != 0) {
            return -1;
        }

        if (buffer[0] == '\0') {
            printf(" Currency cannot be empty.\n");
            continue;
        }

        if (strcasecmp(buffer, "USD") == 0) {
            snprintf(out, size, "USD");
            return 0;
        }
        if (strcasecmp(buffer, "IDR") == 0) {
            snprintf(out, size, "IDR");
            return 0;
        }
        if (strcasecmp(buffer, "EUR") == 0) {
            snprintf(out, size, "EUR");
            return 0;
        }

        int index = market_find_coin(buffer);
        if (index >= 0) {
            const CoinInfo *registry = market_get_registry(NULL);
            snprintf(out, size, "%s", registry[index].symbol);
            return 0;
        }

        printf(" Unsupported currency. Use a symbol from the cryptocurrency list or USD/IDR/EUR.\n");
    }
}

static void ui_pause(void) {
    char buffer[INPUT_SIZE];
    printf("\n Press Enter to continue...");
    fflush(stdout);
    (void)read_line(buffer, sizeof(buffer));
}

static void format_grouped(char *out, size_t size, const char *integer,
                           const char *fraction) {
    if (size == 0) {
        return;
    }

    size_t length = strlen(integer);
    size_t pos = 0;

    for (size_t i = 0; i < length; i++) {
        if (i > 0 && (length - i) % 3 == 0) {
            if (pos + 1 >= size) {
                break;
            }
            out[pos++] = ',';
        }
        if (pos + 1 >= size) {
            break;
        }
        out[pos++] = integer[i];
    }

    if (fraction && fraction[0] != '\0') {
        if (pos + 1 < size) {
            out[pos++] = '.';
        }
        for (size_t i = 0; fraction[i] != '\0'; i++) {
            if (pos + 1 >= size) {
                break;
            }
            out[pos++] = fraction[i];
        }
    }

    out[pos] = '\0';
}

static void format_number(double value, int decimals, int trim_zeros,
                          char *out, size_t size) {
    char raw[64];
    snprintf(raw, sizeof(raw), "%.*f", decimals, value);

    char *dot = strchr(raw, '.');
    char *fraction = NULL;
    if (dot) {
        *dot = '\0';
        fraction = dot + 1;
        if (trim_zeros) {
            size_t length = strlen(fraction);
            while (length > 0 && fraction[length - 1] == '0') {
                fraction[--length] = '\0';
            }
        }
    }

    format_grouped(out, size, raw, fraction);
}

static void print_value(double value, const char *symbol) {
    char number[64];

    if (strcasecmp(symbol, "USD") == 0) {
        format_number(value, 2, 0, number, sizeof(number));
        printf("$%s", number);
    } else if (strcasecmp(symbol, "IDR") == 0) {
        format_number(value, 2, 0, number, sizeof(number));
        printf("Rp%s", number);
    } else if (strcasecmp(symbol, "EUR") == 0) {
        format_number(value, 2, 0, number, sizeof(number));
        printf("€%s", number);
    } else {
        format_number(value, 8, 1, number, sizeof(number));
        printf("%s %s", number, symbol);
    }
}

static void print_rate(double value, const char *symbol) {
    if (strcasecmp(symbol, "USD") == 0 ||
        strcasecmp(symbol, "IDR") == 0 ||
        strcasecmp(symbol, "EUR") == 0) {
        int decimals = (value > 0.0 && value < 0.01) ? 8 : 2;
        char number[64];
        format_number(value, decimals, 1, number, sizeof(number));

        if (strcasecmp(symbol, "USD") == 0) {
            printf("$%s", number);
        } else if (strcasecmp(symbol, "IDR") == 0) {
            printf("Rp%s", number);
        } else {
            printf("€%s", number);
        }
        return;
    }

    print_value(value, symbol);
}

static void print_price_timestamps(const char *from_symbol, const Coin *from_coin,
                                   const char *to_symbol, const Coin *to_coin) {
    const char *from_updated = from_coin ? from_coin->last_updated
                                         : market_get_fiat_updated(from_symbol);
    const char *to_updated = to_coin ? to_coin->last_updated
                                     : market_get_fiat_updated(to_symbol);
    int printed = 0;

    if (from_updated && from_updated[0] != '\0') {
        char from_wib[MAX_TIMESTAMP_LEN];
        format_wib_time(from_updated, true, from_wib, sizeof(from_wib));
        printf("%s %s", from_symbol, from_wib);
        printed = 1;
    }

    if (to_updated && to_updated[0] != '\0') {
        if (printed) {
            printf(" | ");
        }
        char to_wib[MAX_TIMESTAMP_LEN];
        format_wib_time(to_updated, true, to_wib, sizeof(to_wib));
        printf("%s %s", to_symbol, to_wib);
        printed = 1;
    }

    if (!printed) {
        printf("N/A");
    }
}

void display_main_menu(void) {
    printf("\n MAIN MENU\n\n");
    printf("  1. Currency Converter\n");
    printf("  2. Cryptocurrency List\n");
    printf("  3. Market Price\n");
    printf("  4. Conversion History\n");
    printf("  5. Refresh Market Data\n");
    printf("  0. Exit\n");
}

void display_dashboard(void) {
    ui_clear();
    printf("  ██████╗██████╗ ██╗   ██╗██████╗ ███████╗██╗  ██╗\n");
    printf(" ██╔════╝██╔══██╗╚██╗ ██╔╝╚════██╗██╔════╝╚██╗██╔╝\n");
    printf(" ██║     ██████╔╝ ╚████╔╝  █████╔╝█████╗   ╚███╔╝\n");
    printf(" ██║     ██╔══██╗  ╚██╔╝  ██╔═══╝ ██╔══╝   ██╔██╗\n");
    printf(" ╚██████╗██║  ██║   ██║   ███████╗███████╗██╔╝ ██╗\n");
    printf("  ╚═════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝\n");
    printf("\n             CRYPTO CURRENCY CONVERTER\n\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" API STATUS : %s\n", api_status_text());
    printf(" DATA SOURCE: COINPAPRIKA\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    display_main_menu();
    printf("\n%s\n", UI_WIDTH_SEPARATOR);
    printf(" %s\n", COINPAPRIKA_ATTRIBUTION);
    printf("%s\n", UI_WIDTH_SEPARATOR);
}

void display_converter_menu(void) {
    ui_clear();
    printf("%s\n", CONVERTER_SEPARATOR);
    printf("           CURRENCY CONVERTER\n");
    printf("%s\n", CONVERTER_SEPARATOR);
    printf("\n");
}

void display_market_menu(void) {
    ui_clear();
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" MARKET PRICE\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf("\n");
}

void display_history_menu(void) {
    ui_clear();
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" CONVERSION HISTORY\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf("\n");
}

static void run_converter(void) {
    char from_symbol[MAX_SYMBOL_LEN];
    char to_symbol[MAX_SYMBOL_LEN];
    int again = 1;

    while (again) {
        double amount = 0.0;
        display_converter_menu();

        if (read_symbol(" FROM currency (BTC/ETH/USDT/.../USD): ", from_symbol, sizeof(from_symbol)) != 0) {
            return;
        }
        if (read_amount(" Amount: ", &amount) != 0) {
            return;
        }
        if (read_symbol(" TO currency (BTC/ETH/USDT/.../USD): ", to_symbol, sizeof(to_symbol)) != 0) {
            return;
        }

        double from_price = 0.0;
        double to_price = 0.0;

        if (market_get_usd_price(from_symbol, &from_price) != 0) {
            s_api_status = API_STATUS_ERROR;
            printf("\n Failed to fetch %s price. Check your connection.\n", from_symbol);
            ui_pause();
            return;
        }

        if (market_get_usd_price(to_symbol, &to_price) != 0) {
            s_api_status = API_STATUS_ERROR;
            printf("\n Failed to fetch %s price. Check your connection.\n", to_symbol);
            ui_pause();
            return;
        }

        const Coin *from_coin = NULL;
        const Coin *to_coin = NULL;
        if (market_find_coin(from_symbol) >= 0) {
            from_coin = market_get_coin_by_symbol(from_symbol);
        }
        if (market_find_coin(to_symbol) >= 0) {
            to_coin = market_get_coin_by_symbol(to_symbol);
        }

        if (strcasecmp(from_symbol, "USD") != 0 ||
            strcasecmp(to_symbol, "USD") != 0) {
            s_api_status = API_STATUS_CONNECTED;
        }

        double result = 0.0;
        double rate = 0.0;
        if (convert_calculate(amount, from_price, to_price, &result, &rate) != CRYPTEX_OK) {
            printf("\n Conversion failed: invalid amount or price.\n");
            ui_pause();
            return;
        }

        printf("%s\n", CONVERTER_SEPARATOR);
        printf("           CONVERSION RESULT\n");
        printf("%s\n", CONVERTER_SEPARATOR);
        printf(" ");
        print_value(amount, from_symbol);
        printf("\n =\n ");
        print_value(result, to_symbol);
        printf("\n\n Exchange Rate: ");
        print_rate(rate, to_symbol);
        printf(" / %s\n", from_symbol);
        printf(" Price updated: ");
        print_price_timestamps(from_symbol, from_coin, to_symbol, to_coin);
        printf("\n%s\n", CONVERTER_SEPARATOR);

        Conversion conversion = {0};
        snprintf(conversion.from_symbol, sizeof(conversion.from_symbol), "%s", from_symbol);
        snprintf(conversion.to_symbol, sizeof(conversion.to_symbol), "%s", to_symbol);
        conversion.amount = amount;
        conversion.exchange_rate = rate;
        conversion.result = result;

        time_t now = time(NULL);
        struct tm *local = localtime(&now);
        if (local) {
            strftime(conversion.timestamp, sizeof(conversion.timestamp),
                     "%Y-%m-%d %H:%M:%S", local);
        }

        if (history_save(&conversion) != 0) {
            printf(" Warning: failed to save conversion history.\n");
        }

        printf("\n 1. Convert Again\n");
        printf(" 0. Back to Main Menu\n");

        int choice = 0;
        if (read_int(" Choose [0-1]: ", 0, 1, &choice) != 0) {
            return;
        }
        again = (choice == 1);
    }
}

static void show_coin_list(void) {
    size_t count = 0;
    const CoinInfo *registry = market_get_registry(&count);

    ui_clear();
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" CRYPTOCURRENCY LIST\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" %-4s %-8s %-12s %s\n", "#", "SYMBOL", "NAME", "COINPAPRIKA ID");
    printf("%s\n", UI_WIDTH_SEPARATOR);

    for (size_t i = 0; i < count; i++) {
        printf(" %-4zu %-8s %-12s %s\n",
               i + 1, registry[i].symbol, registry[i].name, registry[i].id);
    }

    printf("%s\n", UI_WIDTH_SEPARATOR);
    ui_pause();
}

static void show_market_prices(void) {
    display_market_menu();
    printf(" Fetching market data from CoinPaprika...\n");
    fflush(stdout);

    int status = market_fetch_prices();
    s_api_status = (status == 0) ? API_STATUS_CONNECTED : API_STATUS_ERROR;

    if (status != 0) {
        printf(" Some market data could not be fetched. Showing available data.\n");
    }

    market_display_prices();
    ui_pause();
}

static void show_history(void) {
    display_history_menu();
    history_display();

    printf("\n 1. Clear History\n");
    printf(" 0. Back to Main Menu\n");

    int choice = 0;
    if (read_int(" Choose [0-1]: ", 0, 1, &choice) != 0) {
        return;
    }

    if (choice == 1) {
        int confirm = 0;
        if (read_int(" Clear all history? 1=Yes, 0=No: ", 0, 1, &confirm) != 0) {
            return;
        }

        if (confirm == 1) {
            if (history_clear() != 0) {
                printf(" Failed to clear history.\n");
            } else {
                printf(" History cleared.\n");
            }
            ui_pause();
        }
    }
}

static void refresh_market_data(void) {
    ui_clear();
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf(" REFRESH MARKET DATA\n");
    printf("%s\n", UI_WIDTH_SEPARATOR);
    printf("\n Refreshing prices...\n");
    fflush(stdout);

    int status = market_refresh();
    s_api_status = (status == 0) ? API_STATUS_CONNECTED : API_STATUS_ERROR;

    if (status == 0) {
        printf(" Market data refreshed successfully.\n");
    } else {
        printf(" Refresh failed. Some prices may be unavailable.\n");
    }

    ui_pause();
}

void main_menu(void) {
    int choice = 0;

    while (1) {
        display_dashboard();
        if (read_int("\n Choose menu [0-5]: ", 0, 5, &choice) != 0) {
            printf("\n");
            return;
        }

        switch (choice) {
            case 1:
                run_converter();
                break;
            case 2:
                show_coin_list();
                break;
            case 3:
                show_market_prices();
                break;
            case 4:
                show_history();
                break;
            case 5:
                refresh_market_data();
                break;
            case 0:
                ui_clear();
                printf(" Thank you for using CRYPTEX.\n");
                printf(" %s\n", COINPAPRIKA_ATTRIBUTION);
                return;
            default:
                break;
        }
    }
}
