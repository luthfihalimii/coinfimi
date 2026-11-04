#ifndef CRYPTEX_HISTORY_H
#define CRYPTEX_HISTORY_H

#include "cryptex.h"

int history_initialize(void);
int history_clear(void);
int history_save(const Conversion *conversion);
Conversion *history_load(size_t *out_count);
void history_display(void);

#endif /* CRYPTEX_HISTORY_H */
