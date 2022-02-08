/********************************************************************************************************
 * @file    module_test.c
 *
 * @brief   This is the source file for module_test
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#include "zb_common.h"


#define MODULE_TEST_UART		0

#if MODULE_TEST_UART

#define TEST_826x		0
#define TEST_8258		1
#define TEST_8278		2
#define TEST_9518		3

#define TEST_MODULE		TEST_8258

#if (TEST_MODULE == TEST_826x)
	#define UART_TX_PIN		GPIO_PC2
	#define UART_RX_PIN		GPIO_PC3
#elif (TEST_MODULE == TEST_8258)
	#define UART_TX_PIN		GPIO_PD7
	#define UART_RX_PIN		GPIO_PA0
#elif (TEST_MODULE == TEST_8278)
	#define UART_TX_PIN		GPIO_PD7
	#define UART_RX_PIN		GPIO_PA0
#elif (TEST_MODULE == TEST_9518)
	#define UART_TX_PIN		GPIO_PB2
	#define UART_RX_PIN		GPIO_PB3
#else
	#error	"undefined TEST_MODULE"
#endif

#if (TEST_MODULE == TEST_826x)
	#define UART_PIN_INIT()		do{	\
									drv_uart_pin_set(UART_TX_PIN, UART_RX_PIN);	\
									UART_GPIO_CFG_PC2_PC3();					\
								}while(0)
#else
	#define UART_PIN_INIT()		do{	\
									drv_uart_pin_set(UART_TX_PIN, UART_RX_PIN);	\
								}while(0)
#endif


typedef struct{
	u32 dataLen;
	u8 dataPayload[1];
}uart_rxData_t;

__attribute__((aligned(4))) u8 moduleTest_uartTxBuf[4] = {0};
__attribute__((aligned(4))) u8 moduleTest_uartRxBuf[32] = {0};
volatile u8  T_uartPktSentSeqNo = 0;
volatile u32 T_uartPktRecvSeqNo = 0;
volatile u32 T_uartPktRecvLen = 0;
volatile u32 T_uartPktSentExcept = 0;

void module_test_uartRcvHandler(void){
	/*
	 * the format of the uart rx data: length(4 Bytes) + payload
	 *
	 * */
	uart_rxData_t *rxData = (uart_rxData_t *)moduleTest_uartRxBuf;
	T_uartPktRecvLen = rxData->dataLen;
	T_uartPktRecvSeqNo = rxData->dataPayload[0];

	if(T_uartPktRecvSeqNo == 0){
		drv_uart_tx_start(rxData->dataPayload, T_uartPktRecvLen);
	}

#if (TEST_MODULE == TEST_826x) || (TEST_MODULE == TEST_8258) || (TEST_MODULE == TEST_8278) || (TEST_MODULE == TEST_9518)
	if(T_uartPktRecvSeqNo == 0xBB){
		drv_adc_enable(1);
	}else{
		drv_adc_enable(0);
	}
#endif
}

void moduleTest_forUart(void){
	UART_PIN_INIT();

	drv_uart_init(115200, moduleTest_uartRxBuf, sizeof(moduleTest_uartRxBuf)/sizeof(u8), module_test_uartRcvHandler);

	drv_adc_init();

#if (TEST_MODULE == TEST_826x)
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, NOINPUT);
#elif (TEST_MODULE == TEST_8258)
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, GPIO_PC5);
#elif (TEST_MODULE == TEST_8278)
	//drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, GPIO_PB3);
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, GPIO_PB3);
#elif (TEST_MODULE == TEST_9518)
	drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, ADC_GPIO_PB0);
#endif

	drv_enable_irq();

	for(int i = 0; i < sizeof(moduleTest_uartTxBuf)/sizeof(u8); i++){
		moduleTest_uartTxBuf[i] = i;
	}

	while(1)
	{
		if(T_uartPktRecvSeqNo == 0xAA){
			moduleTest_uartTxBuf[0] = T_uartPktSentSeqNo++;
			if(drv_uart_tx_start(moduleTest_uartTxBuf, sizeof(moduleTest_uartTxBuf)/sizeof(u8)) == 1){
				WaitMs(1000);
			}else{
				T_uartPktSentExcept++;
				while(1);
			}
		}else if(T_uartPktRecvSeqNo == 0xBB){
			u16 vBat = drv_get_adc_data();

			moduleTest_uartTxBuf[0] = HI_UINT16(vBat);
			moduleTest_uartTxBuf[1] = LO_UINT16(vBat);

			if(drv_uart_tx_start(moduleTest_uartTxBuf, 2) == 1){
				WaitMs(1000);
			}else{
				T_uartPktSentExcept++;
				while(1);
			}
		}else if(T_uartPktRecvSeqNo == 0xCC){
			u16 random = zb_random();

			moduleTest_uartTxBuf[0] = HI_UINT16(random);
			moduleTest_uartTxBuf[1] = LO_UINT16(random);

			if(drv_uart_tx_start(moduleTest_uartTxBuf, 2) == 1){
				WaitMs(1000);
			}else{
				T_uartPktSentExcept++;
				while(1);
			}
		}
	}
}

#endif

#define MODULE_TEST_NV		0

#if MODULE_TEST_NV
volatile u8 T_nwkFrmCntError = 0;
volatile u8 T_nwkFrmCntReadErr = 0;
volatile u32 T_frameCnt = 0;
volatile u32 T_readFrm = 0;
volatile u8 T_bufCheck[256] = {0};
void moduleTest_NV(void){
	u8 *pData = (u8*)&g_zbInfo;

	while(0){
		T_readFrm = 0;
		nv_nwkFrameCountSaveToFlash(T_frameCnt);
		T_nwkFrmCntReadErr = nv_nwkFrameCountFromFlash((u32*)&T_readFrm);

		if(T_readFrm != T_frameCnt){
			T_nwkFrmCntError = 1;
			while(1);
		}
		T_frameCnt += 1;
	}

	while(1){
		pData = (u8*)&g_zbInfo;
		for(s32 i = 0; i < sizeof(zb_info_t); i++){
			pData[i] = (i + T_frameCnt);
		}

		nv_flashWriteNew(1, NV_MODULE_ZB_INFO, NV_ITEM_ZB_INFO, sizeof(zb_info_t), (u8*)&g_zbInfo);
		nv_flashReadNew(1, NV_MODULE_ZB_INFO, NV_ITEM_ZB_INFO, sizeof(zb_info_t), (u8*)&T_bufCheck);
		for(s32 i = 0; i < sizeof(zb_info_t); i++){
			if(pData[i] != T_bufCheck[i]){
				while(1);
			}
		}

		nv_flashWriteNew(1, NV_MODULE_ZB_INFO, 2, sizeof(nwk_nib_t), (u8*)&g_zbInfo.nwkNib);
		nv_flashReadNew(1, NV_MODULE_ZB_INFO, 2, sizeof(nwk_nib_t), (u8*)&T_bufCheck);
		pData = (u8*)&g_zbInfo.nwkNib;
		for(s32 i = 0; i < sizeof(nwk_nib_t); i++){
			if(pData[i] != T_bufCheck[i]){
				while(1);
			}
		}
		T_frameCnt += 1;
	}
}
#endif

#define MODULE_TEST_PM 0

#if MODULE_TEST_PM

#if (__PROJECT_TL_SWITCH__)
#define PAD_WAKUPUP_TEST		1
#define TIMER_WAKUPUP_TEST		1

extern drv_pm_pinCfg_t g_switchPmCfg[];
extern void light_on(void);
extern void light_off(void);

unsigned char txPktForPm[48] __attribute__ ((aligned (4))) = {0x09, 0x00, 0x00, 0x00, 0x0a, 0x03, 0x08, 0xd0, 0xff, 0xff, 0xff, 0xff, 0x07};//{0x14,0x00,0x00,0x00,0x15, 0x00, 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff, 0xee, 0xdd, 0xcc, 0xbb};

void moduleTest_PM(void){
	drv_pm_wakeup_src_e wakeupSrc = 0;
	drv_pm_sleep_mode_e mode = 0;

	PM_CLOCK_INIT();

	ZB_RADIO_TX_POWER_SET(0);
	ZB_RADIO_TRX_CFG(144);

	ZB_RADIO_TRX_SWITCH(RF_MODE_TX, 50);

	ZB_RADIO_DMA_HDR_BUILD(txPktForPm, 8);

#if PAD_WAKUPUP_TEST
	drv_pm_wakeupPinConfig(g_switchPmCfg, 2);

	wakeupSrc |= PM_WAKEUP_SRC_PAD;
#endif

	u32 interval = 0;
#if TIMER_WAKUPUP_TEST
	interval = 1000;
	wakeupSrc |= PM_WAKEUP_SRC_TIMER;
#endif

#if defined(MCU_CORE_826x)
	mode = PM_SLEEP_MODE_SUSPEND;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
	mode = PM_SLEEP_MODE_DEEP_WITH_RETENTION;
#endif

	if(wakeupSrc == (PM_WAKEUP_SRC_PAD | PM_WAKEUP_SRC_TIMER)){
		interval = 10000;
	}

	drv_enable_irq();

	WaitUs(1000*1000);

	while(1){
		ZB_RADIO_TX_START(txPktForPm);

		for(u32 i = 0; i < 2; i++){
			light_on();
			WaitUs(100*1000);
			light_off();
			WaitUs(100*1000);
		}

		drv_pm_sleep(mode, wakeupSrc, interval);
	}
}
#else
	#error please compile the project of "sampleSwith"
#endif
#endif

#define MODULE_TEST_RF 0

#if MODULE_TEST_RF

#define TX 1

unsigned char  rx_packet[128]  __attribute__ ((aligned (4)));

unsigned char  tx_packet[48] __attribute__ ((aligned (4))) = {0x09, 0x00, 0x00, 0x00, 0x0a, 0x03, 0x08, 0xd0, 0xff, 0xff, 0xff, 0xff, 0x07};//{0x14,0x00,0x00,0x00,0x15, 0x00, 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff, 0xee, 0xdd, 0xcc, 0xbb};

void moduleTest_RF(void){
	ZB_RADIO_TX_POWER_SET(0);
	ZB_RADIO_TRX_CFG(144);

	ZB_RADIO_RX_BUF_SET(rx_packet);

	drv_enable_irq();

#if TX
	ZB_RADIO_TRX_SWITCH(RF_MODE_TX, 50);

	ZB_RADIO_DMA_HDR_BUILD(tx_packet, 8);

	while(1)
	{
		WaitMs(100);
		ZB_RADIO_TX_START(tx_packet);
	}
#else
	ZB_RADIO_TRX_SWITCH(RF_MODE_RX, 50);
	while(1);
#endif
}
#endif

#define MODULE_TEST_MMOHASH 	0
#if MODULE_TEST_MMOHASH
u8 T_DBG_installCode[18] = {0x83,0xfe,0xd3,0x40,0x7a,0x93,0x97,0x23,0xa5,0xc6,0x39,0xb2,0x69,0x16,0xd5,0x05};
u16 T_DBG_insCodeCRC;
/*
 * expected: "66B6900981E1EE3CA4206B6B861C02BB"
 */
u8 T_DBG_hashOut[16] = {0};

void moudleTest_hash(void){
	T_DBG_insCodeCRC = tl_bdbInstallCodeCRC16(T_DBG_installCode, 16);
	T_DBG_installCode[16] = (u8)(T_DBG_insCodeCRC & 0xff);
	T_DBG_installCode[17] = (u8)(T_DBG_insCodeCRC >> 8);
	ss_mmoHash(T_DBG_installCode, 18, T_DBG_hashOut);
	while(1);
}
#endif

#define MODULE_TEST_CCM	0
#if MODULE_TEST_CCM
//u8 aes_ccmDecTran(u8 micLen, u8 *key, u8 *iv, u8 *mStr, u16 mStrLen, u8 *aStr, u8 aStrLen, u8 *mic)
#if 0
u8 key[] = {0xC0, 0xC1 , 0xC2 , 0xC3 , 0xC4 , 0xC5 , 0xC6 , 0xC7 , 0xC8 , 0xC9 , 0xCa, 0xCb , 0xCc , 0xCd , 0xCe , 0xCf};
u8 nonce[] = {0x21, 0x43, 0x65, 0x87, 0x21, 0x43, 0x65, 0x87, 0x02, 0x00, 0x00, 0x00, 0x05};//0x02, 0x00, 0x00, 0x00, 0x05};
u8 astring[] = {0x8c, 0x10, 0x21, 0x43, 0x65, 0x87, 0x02, 0x00, 0x00, 0x00, 0x20}; //0x02, 0x00, 0x00, 0x00, 0x20};
u8 astrLen = 11;//10;//11;
u8 mstring[] = {0x83};//, 0x20}; //0x02, 0x00, 0x00, 0x00, 0x20};
u8 mstrLen = 0;//1;//11;
u8 mic[] = {0xCF, 0x78, 0x7E, 0x72};// {0xCA, 0x43, 0x24, 0xDD};//{0xF6, 0x36, 0x78, 0x9E}; //{0xCF, 0x78, 0x7E, 0x72};
#else
u8 key[] = {0xC0, 0xC1 , 0xC2 , 0xC3 , 0xC4 , 0xC5 , 0xC6 , 0xC7 , 0xC8 , 0xC9 , 0xCa, 0xCb , 0xCc , 0xCd , 0xCe , 0xCf};
u8 nonce[] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x99, 0x00, 0x00, 0x00, 0x05};
u8 astring[] = {0x8c, 0x12, 0x01, 0x00, 0x00, 0x00, 0x22};
u8 astrLen = 7;
u8 mstring[] = {0x67};
u8 mstrLen = 0;
u8 mic[] = {0x51, 0x3B, 0x81,0x3F}; // {0x1C, 0xF9, 0x06, 0xC0};  //{0xf3, 0xeb, 0x95,0xb1}; //{0x1C, 0xF9, 0x06, 0xC0};
#endif


void moduleTest_ccm(void){
	//aes_ccmAuthTran(4, key, nonce, NULL, mstrLen, astring, astrLen, mic);
	//aes_ccmEncTran(4, key, nonce, NULL, mstrLen, astring, astrLen, mic);

	//while(1);

	aes_ccmDecTran(4, key, nonce, mstring, mstrLen, astring, astrLen, mic);
	//nonce[12] = 0xc5;
	u8 ret = aes_ccmDecAuthTran(4, key, nonce, mstring, mstrLen, astring, astrLen, mic);

	while(1);
}
#endif

#define MODULE_TEST_ADC			0

#if MODULE_TEST_ADC

volatile u16 Cvoltage_value = 0;

void moduleTest_adc(void){
	drv_adc_init();

#if defined(MCU_CORE_826x)
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, NOINPUT);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, GPIO_PB3);
#endif

	drv_adc_enable(1);

	while(1)
	{
		Cvoltage_value = drv_get_adc_data();
		WaitMs(2000);
	}
}
#endif

#define MODULE_TEST_TIMER 0

#if MODULE_TEST_TIMER

#define TEST_826x		0
#define TEST_8258		1
#define TEST_8278		2
#define TEST_9518		3

#define TEST_MODULE		TEST_8258

#if (TEST_MODULE == TEST_826x)
	#define TEST_GPIO_0		GPIO_PB6
	#define TEST_GPIO_1		GPIO_PB4
#elif (TEST_MODULE == TEST_8258)
	#define TEST_GPIO_0		GPIO_PA3
	#define TEST_GPIO_1		GPIO_PA2
#elif (TEST_MODULE == TEST_8278)
	#define TEST_GPIO_0		GPIO_PA3
	#define TEST_GPIO_1		GPIO_PA2
#elif (TEST_MODULE == TEST_9518)
	#define TEST_GPIO_0		GPIO_PB7
	#define TEST_GPIO_1		GPIO_PB6
#else
	#error	"undefined TEST_MODULE"
#endif

volatile u8 moduleTestTimerFlg = 0;
s32 moduleTestTimer0Cb(void *arg){
	static u8 cnt = 0;

	gpio_toggle(TEST_GPIO_0);

	if(++cnt >= 10){
		cnt = 0;
		moduleTestTimerFlg = 1;

		return -1;
	}

	return 0;
}

s32 moduleTestTimer1Cb(void *arg){
	gpio_toggle(TEST_GPIO_1);

	if(moduleTestTimerFlg){
		return 500*1000;
	}

	return 0;
}

void moduleTest_timer(void){
	drv_gpio_func_set(TEST_GPIO_0);
	drv_gpio_output_en(TEST_GPIO_0, 1); 		//enable output
	drv_gpio_input_en(TEST_GPIO_0, 0);			//disable input
	drv_gpio_write(TEST_GPIO_0, 1);              	//LED On

	drv_gpio_func_set(TEST_GPIO_1);
	drv_gpio_output_en(TEST_GPIO_1, 1); 		//enable output
	drv_gpio_input_en(TEST_GPIO_1, 0);			//disable input
	drv_gpio_write(TEST_GPIO_1, 1);              	//LED On

	drv_hwTmr_init(TIMER_IDX_0, TIMER_MODE_SCLK);
	drv_hwTmr_set(TIMER_IDX_0, 500*1000, moduleTestTimer0Cb, NULL);

	drv_hwTmr_init(TIMER_IDX_1, TIMER_MODE_SCLK);
	drv_hwTmr_set(TIMER_IDX_1, 100*1000, moduleTestTimer1Cb, NULL);

	drv_enable_irq();

	while(1){

	}
}
#endif


#define MODULE_TEST_PWM		0
#if MODULE_TEST_PWM

#if (__PROJECT_TL_DIMMABLE_LIGHT__)
void moduleTest_pwm(void){
	PWM_W_CHANNEL_SET();
	drv_pwm_init();

	u8 duty = 5;
	u8 fullDuty = 100;
	u32 max_tick = PWM_CLOCK_SOURCE / 1000;
	u32 cmp_tick = (duty * max_tick) / fullDuty;

	drv_pwm_cfg(PWM_W_CHANNEL, (u16)cmp_tick, (u16)max_tick);
	drv_pwm_start(PWM_W_CHANNEL);

	while(1){
		if(cmp_tick <= max_tick){
			drv_pwm_cfg(PWM_W_CHANNEL, (u16)cmp_tick, (u16)max_tick);
			cmp_tick += 2400;
			WaitMs(500);
		}else{
			cmp_tick = 0;
		}
	}
}
#else
	#error please compile the project of "sampleLight"
#endif
#endif

#define MODULE_TEST_SPI 0

#if MODULE_TEST_SPI
/*********************************************
 *  @brief  SPI interface
 *  -----------------------------------
 *   	 |	SDO  SDI  SCL  CS |
 *  826x |	A2   A3   A4   A5 |
 *  	 |	B5   B6   B7   B4 |
 *  -----------------------------------
 *  	 |	SDO  SDI  SCL  CS |
 *  8258 |	A2   A3   A4   D6 |
 *  	 |	B7   B6   D7   D2 |
 *  -----------------------------------
 *  	 |	SDO  SDI  SCL  CS |
 *  8278 |	A2   A3   A4   D6 |
 *  	 |	B7   B6   D7   D2 |
 *  -----------------------------------
 *  	 |	MOSI MISO SCL  CS |
 *  9518 |	C7   C6   C5   C4 |
 *  	 |	B7   B6   B5   C0 |
 *  -----------------------------------
 */

#if defined(MCU_CORE_826x)
	#define TEST_SPI_PIN_GROUP		SPI_PIN_GPIOA
	#define TEST_SPI_PIN_CS			GPIO_PA5
#elif defined(MCU_CORE_8258)
	#define TEST_SPI_PIN_GROUP		SPI_GPIO_GROUP_A2A3A4D6
	#define TEST_SPI_PIN_CS			GPIO_PD6
#elif defined(MCU_CORE_8278)
	#define TEST_SPI_PIN_SDO		GPIO_PA2
	#define TEST_SPI_PIN_SDI		GPIO_PA3
	#define TEST_SPI_PIN_SCL		GPIO_PA4
	#define TEST_SPI_PIN_CS			GPIO_PD6
#elif defined(MCU_CORE_B91)
	#define TEST_SPI_PIN_MOSI		GPIO_PC7
	#define TEST_SPI_PIN_MISO		GPIO_PC6
	#define TEST_SPI_PIN_SCL		GPIO_PC5
	#define TEST_SPI_PIN_CS			GPIO_PC4
#else
	#error	"undefined"
#endif

#define SPI_MASTER_MODE				0
#define SPI_SLAVE_MODE				1

/* Select test mode. */
#define SPI_MODE					SPI_MASTER_MODE


/* SPI Clock */
#define SPI_CLOCK					400000//400K

/* SPI slave address */
#define SPI_SLAVE_ADDR				0x48000//0x8800
#define SPI_SLAVE_ADDR_LEN			3//2

/* SPI read/write command */
#define SPI_READ_CMD  				0x80
#define SPI_WRITE_CMD 				0x00

#define CMD_BUF_LEN					6
#define DBG_DATA_LEN    			16


u8 cmd_buf[CMD_BUF_LEN] = {0};
//write buff
u8 spi_master_tx_buff[DBG_DATA_LEN] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//read buff
u8 spi_master_rx_buff[DBG_DATA_LEN] = {0x00};




void spi_master_test_init(void){
	drv_spi_master_init(SPI_CLOCK, SPI_MODE_0);

#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
	drv_spi_master_pin_select(TEST_SPI_PIN_GROUP);
#elif defined(MCU_CORE_8278)
	drv_spi_master_pin_select(TEST_SPI_PIN_SCL, TEST_SPI_PIN_CS, TEST_SPI_PIN_SDO, TEST_SPI_PIN_SDI);
#elif defined(MCU_CORE_B91)
	drv_spi_master_pin_select(TEST_SPI_PIN_SCL, TEST_SPI_PIN_CS, TEST_SPI_PIN_MOSI, TEST_SPI_PIN_MISO);
#endif
}

void spi_slave_test_init(void){
	drv_spi_slave_init(SPI_MODE_0);

#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
	drv_spi_slave_pin_select(TEST_SPI_PIN_GROUP);
#elif defined(MCU_CORE_8278)
	drv_spi_slave_pin_select(TEST_SPI_PIN_SCL, TEST_SPI_PIN_CS, TEST_SPI_PIN_SDO, TEST_SPI_PIN_SDI);
#elif defined(MCU_CORE_B91)
	drv_spi_slave_pin_select(TEST_SPI_PIN_SCL, TEST_SPI_PIN_CS, TEST_SPI_PIN_MOSI, TEST_SPI_PIN_MISO);
#endif
}

void spi_master_test(void){
	WaitMs(1000);   //1S

	spi_master_tx_buff[0] += 1;
	spi_master_tx_buff[0] &= 0xff;

	u8 idx = 0;
	for(u8 i = 0; i < SPI_SLAVE_ADDR_LEN; i++){
		cmd_buf[idx++] = (SPI_SLAVE_ADDR >> ((SPI_SLAVE_ADDR_LEN - 1 - i) << 3));
	}

	cmd_buf[idx] = SPI_WRITE_CMD;
	drv_spi_write(cmd_buf, idx + 1, spi_master_tx_buff, DBG_DATA_LEN, TEST_SPI_PIN_CS);

	WaitMs(1000);   //1S

	cmd_buf[idx] = SPI_READ_CMD;
	drv_spi_read(cmd_buf, idx + 1, spi_master_rx_buff, DBG_DATA_LEN, TEST_SPI_PIN_CS);
}

void main_loop(void){
#if(SPI_MODE == SPI_MASTER_MODE)
	spi_master_test();
#else
	WaitMs(50);
#endif
}

void moduleTest_spi(void){
	WaitMs(2000);//leave enough time for SWS_reset when power on

#if(SPI_MODE == SPI_MASTER_MODE)
	 spi_master_test_init();
#else
	 spi_slave_test_init();
#endif

	while(1){
		main_loop();
	}
}
#endif


#define MODULE_TEST_I2C		0

#if MODULE_TEST_I2C

#if	defined(MCU_CORE_826x)
	#define TEST_I2C_PIN_GROUP		I2C_PIN_GPIOA
#elif defined(MCU_CORE_8258)
	#define TEST_I2C_PIN_GROUP		I2C_GPIO_GROUP_A3A4
#elif defined(MCU_CORE_8278)
	#define TEST_I2C_PIN_SDA		I2C_GPIO_SDA_A3
	#define TEST_I2C_PIN_SCL		I2C_GPIO_SCL_A4
#elif defined(MCU_CORE_B91)
	#define TEST_I2C_PIN_SDA		I2C_GPIO_SDA_B3
	#define TEST_I2C_PIN_SCL		I2C_GPIO_SCL_B2
#endif

/* I2C Clock */
#define I2C_CLOCK					200000//200K

/* I2C slave ID */
#define I2C_SLAVE_ID				0x5A
#define I2C_SLAVE_ADDR				0x48000//0x8800
#define I2C_SLAVE_ADDR_LEN			3//2

#define DBG_DATA_LEN				16
//write buff
u8 i2c_master_tx_buff[DBG_DATA_LEN] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//read buff
u8 i2c_master_rx_buff[DBG_DATA_LEN] = {0x00};

void i2c_master_test_init(void){
	drv_i2c_master_init(I2C_CLOCK);

#if	defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
	drv_i2c_gpio_set(TEST_I2C_PIN_GROUP);
#elif defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
	drv_i2c_gpio_set(TEST_I2C_PIN_SDA, TEST_I2C_PIN_SCL);
#endif
}

void i2c_master_test(void){
	WaitMs(1000);   //1S

	gpio_toggle(GPIO_PB7);

	i2c_master_tx_buff[0] += 1;
	i2c_master_tx_buff[0] &= 0xff;

	drv_i2c_write_series(I2C_SLAVE_ID, I2C_SLAVE_ADDR, I2C_SLAVE_ADDR_LEN, i2c_master_tx_buff, DBG_DATA_LEN);

	WaitMs(1000);   //1S

	drv_i2c_read_series(I2C_SLAVE_ID, I2C_SLAVE_ADDR, I2C_SLAVE_ADDR_LEN, i2c_master_rx_buff, DBG_DATA_LEN);
}

void moduleTest_i2c(void){
	WaitMs(2000);//leave enough time for SWS_reset when power on

	drv_gpio_func_set(GPIO_PB7);
	drv_gpio_output_en(GPIO_PB7, 1); 		//enable output
	drv_gpio_input_en(GPIO_PB7, 0);		//disable input
	drv_gpio_write(GPIO_PB7, 1);              	//LED On

	i2c_master_test_init();

	while(1){
		i2c_master_test();
	}
}
#endif


#define MODULE_TEST_GPIO_IRQ	0

#if MODULE_TEST_GPIO_IRQ

#define TEST_826x		0
#define TEST_8258		1
#define TEST_8278		2
#define TEST_9518		3

#define TEST_MODULE		TEST_8258

#if (TEST_MODULE == TEST_826x)
	#define TEST_LED1		GPIO_PC2
	#define TEST_LED2		GPIO_PC3
	#define TEST_SW1		GPIO_PD2
	#define TEST_SW2		GPIO_PC5
	#define TEST_GPIO		GPIO_PD3
#elif (TEST_MODULE == TEST_8258)
	#define TEST_LED1		GPIO_PA4
	#define TEST_LED2		GPIO_PB1
	#define TEST_SW1		GPIO_PD5
	#define TEST_SW2		GPIO_PD6
	#define TEST_GPIO		GPIO_PB2
#elif (TEST_MODULE == TEST_8278)
	#define TEST_LED1		GPIO_PA2
	#define TEST_LED2		GPIO_PA3
	#define TEST_SW1		GPIO_PD6
	#define TEST_SW2		GPIO_PD5
	#define TEST_GPIO		GPIO_PB2
#elif (TEST_MODULE == TEST_9518)
	#define TEST_LED1		GPIO_PB7
	#define TEST_LED2		GPIO_PB6
	#define TEST_SW1		GPIO_PC4
	#define TEST_SW2		GPIO_PC5
	#define TEST_GPIO		GPIO_PA0
#else
	#error	"undefined TEST_MODULE"
#endif


volatile u8 T_DBG_gpioIrqCb1 = 0;
void moduleTest_gpioIrqCb1(void){
	T_DBG_gpioIrqCb1++;
	gpio_toggle(TEST_LED1);
}

volatile u8 T_DBG_gpioIrqCb2 = 0;
void moduleTest_gpioIrqCb2(void){
	T_DBG_gpioIrqCb2++;
	gpio_toggle(TEST_LED2);
}

volatile u8 T_DBG_gpioIrqCb3 = 0;
void moduleTest_gpioIrqCb3(void){
	T_DBG_gpioIrqCb3++;
	gpio_toggle(TEST_LED1);

	if(gpio_read(TEST_GPIO)){
		drv_gpio_up_down_resistor(TEST_GPIO, PM_PIN_PULLUP_10K);
		drv_gpio_irq_config(GPIO_IRQ_MODE, TEST_GPIO, FALLING_EDGE, moduleTest_gpioIrqCb3);
	}else{
		drv_gpio_up_down_resistor(TEST_GPIO, PM_PIN_PULLDOWN_100K);
		drv_gpio_irq_config(GPIO_IRQ_MODE, TEST_GPIO, RISING_EDGE, moduleTest_gpioIrqCb3);
	}
}

volatile u8 T_DBG_mainCnt = 0;
void moduleTest_gpioIrq(void)		//comment out user_init
{
	//1.init the LED pin,for indication
	drv_gpio_func_set(TEST_LED1);
	drv_gpio_output_en(TEST_LED1, 1); 		//enable output
	drv_gpio_input_en(TEST_LED1, 0);		//disable input
	//drv_gpio_write(TEST_LED1, 1);              	//LED On

	drv_gpio_func_set(TEST_LED2);
	drv_gpio_output_en(TEST_LED2, 1); 		//enable output
	drv_gpio_input_en(TEST_LED2, 0);		//disable input
	drv_gpio_write(TEST_LED2, 1);              	//LED On

#if 0
	drv_gpio_func_set(TEST_SW1);
	drv_gpio_output_en(TEST_SW1, 0); 			//enable output
	drv_gpio_input_en(TEST_SW1, 1);				//disable input
	drv_gpio_up_down_resistor(TEST_SW1, PM_PIN_PULLUP_10K);
	drv_gpio_irq_config(GPIO_IRQ_MODE, TEST_SW1, FALLING_EDGE, moduleTest_gpioIrqCb1);
	drv_gpio_irq_en(TEST_SW1);
#endif

#if 0
	drv_gpio_func_set(TEST_SW2);
	drv_gpio_output_en(TEST_SW2, 0); 			//enable output
	drv_gpio_input_en(TEST_SW2, 1);				//disable input
	drv_gpio_up_down_resistor(TEST_SW2, PM_PIN_PULLUP_10K);
	drv_gpio_irq_config(GPIO_IRQ_RISC0_MODE, TEST_SW2, FALLING_EDGE, moduleTest_gpioIrqCb2);
	drv_gpio_irq_risc0_en(TEST_SW2);
#endif

	drv_gpio_func_set(TEST_GPIO);
	drv_gpio_output_en(TEST_GPIO, 0); 			//enable output
	drv_gpio_input_en(TEST_GPIO, 1);			//disable input
	if(gpio_read(TEST_GPIO)){				//test edge trigger irq
		drv_gpio_up_down_resistor(TEST_GPIO, PM_PIN_PULLUP_10K);
		drv_gpio_irq_config(GPIO_IRQ_MODE, TEST_GPIO, FALLING_EDGE, moduleTest_gpioIrqCb3);
		drv_gpio_write(TEST_LED1, 1);
	}else{
		drv_gpio_up_down_resistor(TEST_GPIO, PM_PIN_PULLDOWN_100K);
		drv_gpio_irq_config(GPIO_IRQ_MODE, TEST_GPIO, RISING_EDGE, moduleTest_gpioIrqCb3);
		drv_gpio_write(TEST_LED1, 0);
	}
	drv_gpio_irq_en(TEST_GPIO);

	drv_enable_irq();

	while(1){
		T_DBG_mainCnt++;
		gpio_toggle(TEST_LED2);
		WaitMs(500);
	};
}
#endif


void moduleTest_start(void){
#if MODULE_TEST_MMOHASH
	moudleTest_hash();
#endif

#if MODULE_TEST_CCM
	moduleTest_ccm();
#endif

#if MODULE_TEST_PM
	moduleTest_PM();
#endif

#if MODULE_TEST_UART
	moduleTest_forUart();
#endif

#if MODULE_TEST_RF
	moduleTest_RF();
#endif

#if MODULE_TEST_ADC
	moduleTest_adc();
#endif

#if MODULE_TEST_TIMER
	moduleTest_timer();
#endif

#if MODULE_TEST_PWM
	moduleTest_pwm();
#endif

#if MODULE_TEST_SPI
	moduleTest_spi();
#endif

#if MODULE_TEST_I2C
	moduleTest_i2c();
#endif

#if MODULE_TEST_NV
	moduleTest_NV();
#endif

#if MODULE_TEST_GPIO_IRQ
	moduleTest_gpioIrq();
#endif
}



