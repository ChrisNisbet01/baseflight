#pragma once

#define DIVIDE_WITH_ROUNDING( n, d ) (n) < 0 ? ((n)-((d)/2))/(d) : ((n)+((d)/2))/(d)

int constrain(int amt, int low, int high);
// sensor orientation
void alignSensors(int16_t *src, int16_t *dest, uint8_t rotation);
void initBoardAlignment(void);
void productionDebug(void);
