#include "utils/time_utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char out[32];

    assert(format_wib_time("2026-11-04T05:04:14Z", true, out, sizeof(out)) == 0);
    assert(strcmp(out, "2026-11-04 12:04:14 WIB") == 0);

    assert(format_wib_time("2026-11-04T05:04:14Z", false, out, sizeof(out)) == 0);
    assert(strcmp(out, "2026-11-04 12:04 WIB") == 0);

    assert(format_wib_time("2026-11-04T20:30:00Z", true, out, sizeof(out)) == 0);
    assert(strcmp(out, "2026-11-05 03:30:00 WIB") == 0);

    assert(format_wib_time("2026-12-31T18:00:00Z", true, out, sizeof(out)) == 0);
    assert(strcmp(out, "2027-01-01 01:00:00 WIB") == 0);

    assert(format_wib_time("2024-02-28T18:00:00Z", true, out, sizeof(out)) == 0);
    assert(strcmp(out, "2024-02-29 01:00:00 WIB") == 0);

    assert(format_wib_time("2023-02-28T18:00:00Z", true, out, sizeof(out)) == 0);
    assert(strcmp(out, "2023-03-01 01:00:00 WIB") == 0);

    assert(format_wib_time("invalid", true, out, sizeof(out)) != 0);
    assert(strcmp(out, "invalid") == 0);

    assert(format_wib_time("2026-13-01T00:00:00Z", true, out, sizeof(out)) != 0);
    assert(strcmp(out, "2026-13-01T00:00:00Z") == 0);

    printf("[TEST] test_time_utils PASSED.\n");
    return 0;
}
