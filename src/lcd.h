#pragma once

#include <stdbool.h>
#include <stdint.h>

void init_lcd();
void init_lcd_deepsleep();
void send_to_lcd(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
void update_lcd();
void show_temp_symbol(u8 symbol);
void show_battery_symbol(bool state);
void show_big_number(int16_t number, bool point);
void show_small_number(u16 number, bool percent);
void show_smiley(u8 state);
void show_atc();
void show_sto();
void show_atc_mac();
void show_ble_symbol(bool state);
void send_to_lcd_long(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
void uart_send_lcd(u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6);
