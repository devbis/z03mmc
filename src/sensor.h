#pragma once

#include <stdint.h>

void init_sensor();
void read_sensor(int16_t *temp, u16 *humi);

