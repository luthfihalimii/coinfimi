#include "history.h"
#include <stdio.h>
#include <sys/stat.h>

int history_initialize(void) {
    /* Ensure data directory exists */
    struct stat st;
    if (stat("data", &st) == -1) {
        mkdir("data", 0755);
    }

    FILE *f = fopen(CRYPTEX_HISTORY_FILE, "r");
    if (!f) {
        f = fopen(CRYPTEX_HISTORY_FILE, "w");
        if (f) {
            fprintf(f, "timestamp,from_symbol,to_symbol,amount,exchange_rate,result\n");
            fclose(f);
        } else {
            return -1;
        }
    } else {
        fclose(f);
    }
    return 0;
}

int history_save(const Conversion *conversion) {
    if (!conversion) return -1;
    FILE *f = fopen(CRYPTEX_HISTORY_FILE, "a");
    if (!f) return -1;

    fprintf(f, "%s,%s,%s,%.8f,%.8f,%.8f\n",
            conversion->timestamp,
            conversion->from_symbol,
            conversion->to_symbol,
            conversion->amount,
            conversion->exchange_rate,
            conversion->result);
    fclose(f);
    return 0;
}

void history_display(void) {
    /* Implemented in Phase 4 */
    printf("[History] Display placeholder...\n");
}
