/********************************************************************************************************
 * @file    bootloader.c
 *
 * @brief   This is the source file for bootloader
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

#if (__PROJECT_TL_BOOT_LOADER__)

#include "tl_common.h"
#include "bootloader.h"



/* FLASH address */
#define APP_RUNNING_ADDR				APP_IMAGE_ADDR
#define APP_NEW_IMAGE_ADDR				FLASH_ADDR_OF_OTA_IMAGE

/* SRAM address */
#if defined(MCU_CORE_826x)
	#define MCU_RAM_START_ADDR			0x808000
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define MCU_RAM_START_ADDR			0x840000
#elif defined(MCU_CORE_B91)
	#define MCU_RAM_START_ADDR			0x0000
#endif

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define REBOOT()					WRITE_REG8(0x602, 0x88)
#elif defined(MCU_CORE_B91)
	#define REBOOT()					((void(*)(void))(FLASH_R_BASE_ADDR + APP_IMAGE_ADDR))()
#endif

#define FW_START_UP_FLAG				0x4B
#define FW_RAMCODE_SIZE_MAX				RESV_FOR_APP_RAM_CODE_SIZE



static bool is_valid_fw_bootloader(u32 addr_fw){
	u8 startup_flag = 0;
    flash_read(addr_fw + FLASH_TLNK_FLAG_OFFSET, 1, &startup_flag);

    return ((startup_flag == FW_START_UP_FLAG) ? TRUE : FALSE);
}

void bootloader_with_ota_check(u32 addr_load, u32 new_image_addr){
	if(new_image_addr != addr_load){
		if(is_valid_fw_bootloader(new_image_addr)){
			u8 buf[256];

			flash_read(new_image_addr, 256, buf);
			u32 fw_size = *(u32 *)(buf + 0x18);

			if(fw_size <= FLASH_OTA_IMAGE_MAX_SIZE){
				for(int i = 0; i < fw_size; i += 256){
					if((i & 0xfff) == 0){
						flash_erase(addr_load + i);
					}

					flash_read(new_image_addr + i, 256, buf);
					flash_write(addr_load + i, 256, buf);
				}
			}

			buf[0] = 0;
			flash_write(new_image_addr + FLASH_TLNK_FLAG_OFFSET, 1, buf);   //clear OTA flag

			//erase the new firmware
			for(int i = 0; i < ((fw_size + 4095)/4096); i++) {
				flash_erase(new_image_addr + i*4096);
			}
		}
	}

    if(is_valid_fw_bootloader(addr_load)){
#if !defined(MCU_CORE_B91)
    	u32 ramcode_size = 0;
        flash_read(addr_load + 0x0c, 2, (u8 *)&ramcode_size);
        ramcode_size *= 16;

        if(ramcode_size > FW_RAMCODE_SIZE_MAX){
            ramcode_size = FW_RAMCODE_SIZE_MAX; // error, should not run here
        }
        flash_read(addr_load, ramcode_size, (u8 *)MCU_RAM_START_ADDR); // copy ram code
#endif
        REBOOT();
    }
}

void bootloader_init(void){
	bootloader_with_ota_check(APP_RUNNING_ADDR, APP_NEW_IMAGE_ADDR);
}

#endif	/* __PROJECT_TL_BOOT_LOADER__ */
