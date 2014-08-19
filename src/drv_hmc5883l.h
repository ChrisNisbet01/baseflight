#pragma once

bool hmc5883lDetect(sensor_align_e align);
void hmc5883lInit(void);
void hmc5883lRead(int32_t *magData);
