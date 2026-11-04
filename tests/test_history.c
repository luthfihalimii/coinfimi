#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cryptex.h"
#include "history/history.h"

static char s_original_dir[1024];
static char s_temp_dir[1024];

static void setup_temp_dir(void) {
    char template[] = "/tmp/cryptex_history_XXXXXX";
    assert(getcwd(s_original_dir, sizeof(s_original_dir)) != NULL);
    assert(mkdtemp(template) != NULL);
    snprintf(s_temp_dir, sizeof(s_temp_dir), "%s", template);
    assert(chdir(s_temp_dir) == 0);
}

static void teardown_temp_dir(void) {
    char path[1200];

    snprintf(path, sizeof(path), "%s/data/history.csv", s_temp_dir);
    (void)unlink(path);
    snprintf(path, sizeof(path), "%s/data", s_temp_dir);
    (void)rmdir(path);

    assert(chdir(s_original_dir) == 0);
    (void)rmdir(s_temp_dir);
}

static void test_initialize_creates_file(void) {
    assert(history_initialize() == 0);

    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "r");
    assert(file != NULL);

    char line[128];
    assert(fgets(line, sizeof(line), file) != NULL);
    assert(strstr(line, "timestamp,from_symbol,to_symbol,amount,exchange_rate,result") != NULL);
    fclose(file);
}

static void test_empty_history(void) {
    size_t count = 99;
    Conversion *items = history_load(&count);
    assert(items == NULL);
    assert(count == 0);
}

static void test_save_and_load(void) {
    Conversion input = {0};
    snprintf(input.from_symbol, sizeof(input.from_symbol), "BTC");
    snprintf(input.to_symbol, sizeof(input.to_symbol), "USD");
    snprintf(input.timestamp, sizeof(input.timestamp), "2026-11-04 10:00:00");
    input.amount = 0.5;
    input.exchange_rate = 95000.0;
    input.result = 47500.0;

    assert(history_save(&input) == 0);

    size_t count = 0;
    Conversion *items = history_load(&count);
    assert(items != NULL);
    assert(count == 1);
    assert(strcmp(items[0].from_symbol, "BTC") == 0);
    assert(strcmp(items[0].to_symbol, "USD") == 0);
    assert(strcmp(items[0].timestamp, "2026-11-04 10:00:00") == 0);
    assert(items[0].amount == 0.5);
    assert(items[0].exchange_rate == 95000.0);
    assert(items[0].result == 47500.0);
    free(items);

    history_display();
}

static void test_clear_history(void) {
    assert(history_clear() == 0);

    size_t count = 99;
    Conversion *items = history_load(&count);
    assert(items == NULL);
    assert(count == 0);

    FILE *file = fopen(CRYPTEX_HISTORY_FILE, "r");
    assert(file != NULL);

    char line[128];
    assert(fgets(line, sizeof(line), file) != NULL);
    assert(strstr(line, "timestamp,from_symbol,to_symbol,amount,exchange_rate,result") != NULL);
    fclose(file);
}

static void test_missing_file(void) {
    assert(unlink(CRYPTEX_HISTORY_FILE) == 0);

    size_t count = 99;
    Conversion *items = history_load(&count);
    assert(items == NULL);
    assert(count == 0);
}

int main(void) {
    printf("[TEST] Running test_history...\n");

    setup_temp_dir();
    test_initialize_creates_file();
    test_empty_history();
    test_save_and_load();
    test_clear_history();
    test_missing_file();
    teardown_temp_dir();

    printf("[TEST] test_history PASSED.\n");
    return 0;
}
