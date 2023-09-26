#include "compiler.h"
#include "tl_common.h"

#include "battery.h"

uint8_t adc_hw_initialized = 0;
#define ADC_BUF_COUNT	8

// Process takes about 120 μs at CPU CLK 24Mhz.
_attribute_ram_code_
static void adc_channel_init(ADC_InputPchTypeDef p_ain) {
	adc_set_sample_clk(5);
	adc_set_left_right_gain_bias(GAIN_STAGE_BIAS_PER100, GAIN_STAGE_BIAS_PER100);
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);
	adc_set_state_length(240, 0, 10);
	analog_write(anareg_adc_res_m, RES14 | FLD_ADC_EN_DIFF_CHN_M);
	adc_set_ain_chn_misc(p_ain, GND);
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);
	adc_set_tsample_cycle_chn_misc(SAMPLING_CYCLES_6);
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
}

// Process takes about 260 μs at CPU CLK 24Mhz.
_attribute_ram_code_
uint16_t get_adc_mv(uint32_t p_ain) { // ADC_InputPchTypeDef
	volatile unsigned int adc_dat_buf[ADC_BUF_COUNT];
	uint16_t temp;
	int i, j;
	if (adc_hw_initialized != p_ain) {
		adc_hw_initialized = p_ain;
		adc_power_on_sar_adc(0);
#if 0 // gpio set in board_8258_03mmc.h
		if(p_ain == SHL_ADC_VBAT) {
			// Set missing pin on case TLSR8251F512ET24/TLSR8253F512ET32
			gpio_set_output_en(GPIO_VBAT, 1);
			gpio_set_input_en(GPIO_VBAT, 0);
			gpio_write(GPIO_VBAT, 1);
		}
#endif
		adc_channel_init(p_ain);
	}
	adc_power_on_sar_adc(1); // + 0.4 mA
	adc_reset_adc_module();
	u32 t0 = clock_time();

	uint16_t adc_sample[ADC_BUF_COUNT]; // = { 0 };
	u32 adc_average;
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		adc_dat_buf[i] = 0;
	}
	while (!clock_time_exceed(t0, 25)); //wait at least 2 sample cycle(f = 96K, T = 10.4us)
	adc_config_misc_channel_buf((uint16_t *) adc_dat_buf, sizeof(adc_dat_buf));
	dfifo_enable_dfifo2();
	sleep_us(20);
	for (i = 0; i < ADC_BUF_COUNT; i++) {
		while (!adc_dat_buf[i]);
		if (adc_dat_buf[i] & BIT(13)) {
			adc_sample[i] = 0;
		} else {
			adc_sample[i] = ((uint16_t) adc_dat_buf[i] & 0x1FFF);
		}
		if (i) {
			if (adc_sample[i] < adc_sample[i - 1]) {
				temp = adc_sample[i];
				adc_sample[i] = adc_sample[i - 1];
				for (j = i - 1; j >= 0 && adc_sample[j] > temp; j--) {
					adc_sample[j + 1] = adc_sample[j];
				}
				adc_sample[j + 1] = temp;
			}
		}
	}
	dfifo_disable_dfifo2();
	adc_power_on_sar_adc(0); // - 0.4 mA
	adc_average = (adc_sample[2] + adc_sample[3] + adc_sample[4]
			+ adc_sample[5]) / 4;
#if DEVICE_TYPE == DEVICE_MJWSD05MMC
	return (adc_average * 1686) >> 10; // adc_vref default: 1175 (mV)
#else
	return (adc_average * 1175) >> 10; // adc_vref default: 1175 (mV)
#endif
}

// 2200..3100 mv - 0..100%
_attribute_ram_code_
uint8_t get_battery_level(uint16_t battery_mv) {
	uint8_t battery_level = 0;
	if (battery_mv > MIN_VBAT_MV) {
		battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV
				- MIN_VBAT_MV) / 100);
		if (battery_level > 100)
			battery_level = 100;
	}
	return battery_level;
}


_attribute_data_retention_ struct {
	uint32_t buf2[BAT_AVERAGE_COUNT];
	uint16_t buf1[BAT_AVERAGE_COUNT];
	uint8_t index1;
	uint8_t index2;
} bat_average;

_attribute_data_retention_ battery_data_t battery_data;

void low_vbat(void) {
    //	go_sleep(120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
}

//__attribute__((optimize("-Os")))
_attribute_ram_code_
void check_battery(void) {
	uint32_t i;
	uint32_t summ = 0;
	battery_data.battery_mv = get_battery_mv();
	if (battery_data.battery_mv < END_VBAT_MV) // It is not recommended to write Flash below 2V
		low_vbat();
	bat_average.index1++;
	bat_average.index1 &= BAT_AVERAGE_COUNT - 1;
	if(bat_average.index1 == 0) {
		bat_average.index2++;
		bat_average.index2 &= BAT_AVERAGE_COUNT - 1;
	}
	bat_average.buf1[bat_average.index1] = battery_data.battery_mv;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf1[i];
	bat_average.buf2[bat_average.index2] = summ;
	summ = 0;
	for(i = 0; i < BAT_AVERAGE_COUNT; i++)
		summ += bat_average.buf2[i];
	battery_data.average_battery_mv = summ >> (2*BAT_AVERAGE_SHL);
	battery_data.battery_level = get_battery_level(battery_data.battery_mv);
}