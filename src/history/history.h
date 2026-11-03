#ifndef CRYPTEX_HISTORY_H
#define CRYPTEX_HISTORY_H

#include "cryptex.h"

int history_initialize(void);
int history_save(const Conversion *conversion);
void history_display(void);

#endif /* CRYPTEX_HISTORY_H */
