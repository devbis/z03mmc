#pragma once

#include <stdint.h>

#define SHL_ADC_VBAT	1  // "B0P" in sdk/platform/adc.h

#define MAX_VBAT_MV		2950 // 3100 mV - > battery = 100% no load, 2950 at load (during measurement)
#define MIN_VBAT_MV		2200 // 2200 mV - > battery = 0%

#define LOW_VBAT_MV		2800 // level set LOW_CONNECT_LATENCY
#define END_VBAT_MV		2000 // It is not recommended to write Flash below 2V, go to deep-sleep

uint16_t get_adc_mv(uint32_t p_ain);

#define get_battery_mv() get_adc_mv(SHL_ADC_VBAT)	// Channel B0P/B5P

#define BAT_AVERAGE_SHL		4 // 16*16 = 256 ( 256*10/60 = 42.7 min)
#define BAT_AVERAGE_COUNT	(1 << BAT_AVERAGE_SHL) // 8

typedef struct _battery_data_t {
	uint16_t 	average_battery_mv; // mV
	uint16_t	battery_mv; // mV
	uint8_t 	battery_level;	// 0..100% (average_battery_mv)
} battery_data_t;

void check_battery(void);
