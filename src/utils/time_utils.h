#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stdbool.h>
#include <stddef.h>

int format_wib_time(const char *iso_utc, bool include_seconds, char *out, size_t size);

#endif
