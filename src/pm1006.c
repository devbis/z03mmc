#include "zb_common.h"
#include "drv_uart.h"

#include "pm1006.h"
#include "app_cfg.h"
#include "device.h"

typedef struct{
	u32 dma_len;        // dma len must be 4 byte
	u8 data[1];
} uart_data_t;


__attribute__((aligned(4))) u8 uartRxBuf[UART_RX_BUF_SIZE] = {0};

extern zcl_concentrationAttr_t g_zcl_pm25Attrs;
extern zcl_concentrationAttr_t g_zcl_pm1Attrs;
extern zcl_concentrationAttr_t g_zcl_pm10Attrs;

void uartRcvHandler(void){
    // printf("[pm1006] uartRcvHandler\r\n");
    uart_data_t *rec_buff = (uart_data_t *)uartRxBuf;

    // printf("[pm1006] rec_buff.dma_len = %d\r\n", rec_buff->dma_len);

    if (rec_buff->dma_len == 20) {
        // b'\x16\x11\x0b\x00\x00\x00\r\x00\x00\x03\xc5\x00\x00\x00#\x02\x00\x00\x16\xbe'
        // sample data:
        //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
        // 16 11 0b 00 00 00 0c 00 00 03 cb 00 00 00 0c 01 00 00 00 e7
        //               |pm2_5|     |pm1_0|     |pm10 |        | CRC |

        u16 pm25 = (rec_buff->data[5] << 8) | rec_buff->data[6];
        u16 pm1 = (rec_buff->data[9] << 8) | rec_buff->data[10];
        u16 pm10 = (rec_buff->data[13] << 8) | rec_buff->data[14];

        // printf(".. pm25: 0x%x, pm1: 0x%x, pm10: 0x%x\r\n", pm25, pm1, pm10);
        g_zcl_pm25Attrs.measuredValue = (float) pm25;
        g_zcl_pm1Attrs.measuredValue = ((float) pm1) / 100.0;
        g_zcl_pm10Attrs.measuredValue = (float) pm10;
    }

	rec_buff->dma_len = 0;
}

void pm1006_uart_init(void) {
    uart_gpio_set(UART_TX_PIN, UART_RX_PIN);
	drv_uart_init(9600, uartRxBuf, UART_RX_BUF_SIZE, uartRcvHandler);
}
