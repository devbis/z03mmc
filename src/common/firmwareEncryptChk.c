/********************************************************************************************************
 * @file    firmwareEncryptChk.c
 *
 * @brief   This is the source file for firmwareEncryptChk
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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
 *
 *******************************************************************************************************/

#include "tl_common.h"
#if UID_ENABLE
#include "firmware_encrypt.h"
#endif
#include "firmwareEncryptChk.h"


/**
 *  @brief Only support for 8258/8278/b91, if you want to this function, please contact to us.
 */
u8 firmwareCheckWithUID(void)
{
#if UID_ENABLE
	u32 flash_mid = 0;
	u8 flash_uid[16] = {0};
	int flag = flash_read_mid_uid_with_check(&flash_mid, flash_uid);
	if(flag == 0){
		return 1;
	}
	u8 ciphertext[16] = {0};
	firmware_encrypt_based_on_uid(flash_uid, ciphertext);

	u8 code[16] = {0};
	flash_read(CFG_FIRMWARE_ENCRYPTION, 16, code);

	if(memcmp(ciphertext, code, 16)){
		return 1;
	}
#endif

	return 0;
}
