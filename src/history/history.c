#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define HISTORY_LINE_SIZE 256
#define HISTORY_INITIAL_CAPACITY 16
#define HISTORY_DISPLAY_LIMIT 20
#define HISTORY_SEPARATOR "------------------------------------------------------------------------------"
#define HISTORY_HEADER "timestamp,from_symbol,to_symbol,amount,exchange_rate,result\n"

int history_initialize(void) {
    struct stat st;
    if (stat("data", &st) == -1) {
        if (mkdir("data", 0755) != 0) {
            return -1;
        }
    }

    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "r");
    if (!file) {
        file = fopen(CRYPTEX_HISTORY_FILE, "w");
        if (!file) {
            return -1;
        }
        fprintf(file, "%s", HISTORY_HEADER);
        fclose(file);
    } else {
        fclose(file);
    }
    return 0;
}

int history_clear(void) {
    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "w");
    if (!file) {
        return -1;
    }

    int written = fprintf(file, "%s", HISTORY_HEADER);
    int close_status = fclose(file);

    if (written < 0 || close_status != 0) {
        return -1;
    }
    return 0;
}

int history_save(const Conversion *conversion) {
    if (!conversion) {
        return -1;
    }

    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "a");
    if (!file) {
        return -1;
    }

    int written = fprintf(file, "%s,%s,%s,%.8f,%.8f,%.8f\n",
                          conversion->timestamp,
                          conversion->from_symbol,
                          conversion->to_symbol,
                          conversion->amount,
                          conversion->exchange_rate,
                          conversion->result);
    int close_status = fclose(file);

    if (written < 0 || close_status != 0) {
        return -1;
    }
    return 0;
}

Conversion *history_load(size_t *out_count) {
    if (out_count) {
        *out_count = 0;
    }

    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "r");
    if (!file) {
        return NULL;
    }

    Conversion *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char line[HISTORY_LINE_SIZE];

    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        Conversion conversion;
        memset(&conversion, 0, sizeof(conversion));

        int fields = sscanf(line, "%31[^,],%15[^,],%15[^,],%lf,%lf,%lf",
                            conversion.timestamp,
                            conversion.from_symbol,
                            conversion.to_symbol,
                            &conversion.amount,
                            &conversion.exchange_rate,
                            &conversion.result);
        if (fields != 6) {
            continue;
        }

        if (count == capacity) {
            size_t new_capacity = (capacity == 0) ? HISTORY_INITIAL_CAPACITY : capacity * 2;
            Conversion *new_items = realloc(items, new_capacity * sizeof(*items));
            if (!new_items) {
                free(items);
                fclose(file);
                return NULL;
            }
            items = new_items;
            capacity = new_capacity;
        }

        items[count++] = conversion;
    }

    fclose(file);

    if (out_count) {
        *out_count = count;
    }
    return items;
}

void history_display(void) {
    size_t count = 0;
    Conversion *items = history_load(&count);

    printf("%s\n", HISTORY_SEPARATOR);
    if (!items || count == 0) {
        printf(" No conversion history yet.\n");
        printf("%s\n", HISTORY_SEPARATOR);
        free(items);
        return;
    }

    size_t start = (count > HISTORY_DISPLAY_LIMIT) ? count - HISTORY_DISPLAY_LIMIT : 0;
    if (start > 0) {
        printf(" Showing last %d of %zu conversions.\n", HISTORY_DISPLAY_LIMIT, count);
        printf("%s\n", HISTORY_SEPARATOR);
    }

    printf(" %-19s %-12s %14s %14s %14s\n",
           "TIMESTAMP", "PAIR", "AMOUNT", "RATE", "RESULT");
    printf("%s\n", HISTORY_SEPARATOR);

    for (size_t i = start; i < count; i++) {
        char pair[40];
        snprintf(pair, sizeof(pair), "%s -> %s", items[i].from_symbol, items[i].to_symbol);
        printf(" %-19s %-12s %14.8f %14.8f %14.8f\n",
               items[i].timestamp,
               pair,
               items[i].amount,
               items[i].exchange_rate,
               items[i].result);
    }

    printf("%s\n", HISTORY_SEPARATOR);
    printf(" Total conversions: %zu\n", count);
    free(items);
}
