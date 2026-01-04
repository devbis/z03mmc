#pragma once

#include <stdbool.h>
#include <stdint.h>

enum {
    LCD_I2C_3C = 0x01, // B1.4
    LCD_I2C_3E = 0x02, // B1.9
    LCD_UART = 0x03, // B1.6 old
    LCD_SPI = 0x04, // B1.6 new
};

#define CLK_DELAY_US	32 // =24 -> 20 kHz, =48 -> 10 kHz


void init_lcd(bool clear);
void init_lcd_deepsleep();
void send_to_lcd(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
void update_lcd();
void show_temp_symbol(u8 symbol);
void show_battery_symbol(bool state);
void show_big_number(int16_t number, bool point);
void show_small_number(u16 number, bool percent);
void show_smiley(u8 state);
void show_zigbe();
void show_atc();
void show_sto();
void show_atc_mac();
void show_ble_symbol(bool state);
void send_to_lcd_long(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
void uart_send_lcd(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
