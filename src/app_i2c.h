#pragma once

void init_i2c();
void send_i2c(u8 device_id, const u8 *buffer, int dataLen);
u8 test_i2c_device(u8 address);

