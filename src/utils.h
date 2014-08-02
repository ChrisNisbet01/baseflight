#pragma once

#define DIVIDE_WITH_ROUNDING( n, d ) (n) < 0 ? ((n)-((d)/2))/(d) : ((n)+((d)/2))/(d)

int constrain(int amt, int low, int high);
// sensor orientation
void alignSensors(sensor_data_t *src, sensor_data_t *dest, sensor_align_e rotation);
void initBoardAlignment(void);
void productionDebug(void);
