#pragma once

enum {
	SENSOR_UNSET = 0x0,
	SENSOR_SHTC3 = 0x01, // address 0x70/0xE0 // B1.4
	SENSOR_SHV4 = 0x02, // address 0x44/0x88 // B1.6 and B1.9
};


void init_sensor();
void read_sensor(s16 *temp, u16 *humi);

