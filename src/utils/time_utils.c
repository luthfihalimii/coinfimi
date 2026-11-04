#include "time_utils.h"
#include <stdio.h>

static bool is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static int days_in_month(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

int format_wib_time(const char *iso_utc, bool include_seconds, char *out, size_t size) {
    if (!out || size == 0) {
        return -1;
    }

    if (!iso_utc || iso_utc[0] == '\0') {
        out[0] = '\0';
        return -1;
    }

    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int consumed = 0;

    int fields = sscanf(iso_utc, "%4d-%2d-%2dT%2d:%2d:%2dZ%n",
                        &year, &month, &day, &hour, &minute, &second, &consumed);
    if (fields != 6 || iso_utc[consumed] != '\0' ||
        year < 1 || month < 1 || month > 12 ||
        day < 1 || day > days_in_month(year, month) ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59) {
        snprintf(out, size, "%s", iso_utc);
        return -1;
    }

    hour += 7;
    if (hour >= 24) {
        hour -= 24;
        day++;
        if (day > days_in_month(year, month)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }

    if (include_seconds) {
        snprintf(out, size, "%04d-%02d-%02d %02d:%02d:%02d WIB",
                 year, month, day, hour, minute, second);
    } else {
        snprintf(out, size, "%04d-%02d-%02d %02d:%02d WIB",
                 year, month, day, hour, minute);
    }

    return 0;
}
