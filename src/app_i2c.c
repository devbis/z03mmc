#include "tl_common.h"
#include "types.h"
#include "app_cfg.h"

_attribute_data_retention_ bool i2c_sending;

void init_i2c(){
	i2c_gpio_set(I2C_GPIO_GROUP);
	drv_i2c_master_init(I2C_CLOCK);
}

void send_i2c(u8 device_id, const u8 *buffer, int dataLen){
	if(i2c_sending)return;
	i2c_sending=true;
	i2c_set_id(device_id);
	i2c_write_series(0, 0, (u8*)buffer,dataLen);
	i2c_sending=false;
}

u8 test_i2c_device(u8 address){
	reg_i2c_id = address << 1;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY);
	reg_i2c_ctrl = FLD_I2C_CMD_STOP;
	while(reg_i2c_status & FLD_I2C_CMD_BUSY	);

    return (reg_i2c_status & FLD_I2C_NAK) ? 0 : 1;
}