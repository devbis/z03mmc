/********************************************************************************************************
 * @file    factory_reset.c
 *
 * @brief   This is the source file for factory_reset
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
#include "factory_reset.h"
#include "zb_api.h"

#define FACTORY_RESET_POWER_CNT_THRESHOLD		10	//times
#define FACTORY_RESET_TIMEOUT					2	//second

ev_timer_event_t *factoryRst_timerEvt = NULL;
u8 factoryRst_powerCnt = 0;
bool factoryRst_exist = FALSE;

nv_sts_t factoryRst_powerCntSave(void){
	nv_sts_t st = NV_SUCC;
#if NV_ENABLE
	st = nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_POWER_CNT, 1, &factoryRst_powerCnt);
#else
	st = NV_ENABLE_PROTECT_ERROR;
#endif
	return st;
}

nv_sts_t factoryRst_powerCntRestore(void){
	nv_sts_t st = NV_SUCC;
#if NV_ENABLE
	st = nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_POWER_CNT, 1, &factoryRst_powerCnt);
#else
	st = NV_ENABLE_PROTECT_ERROR;
#endif
	return st;
}

static s32 factoryRst_timerCb(void *arg){
	if(factoryRst_powerCnt >= FACTORY_RESET_POWER_CNT_THRESHOLD){
		/* here is just a mark, wait for device announce and then perform factory reset. */
		factoryRst_exist = TRUE;
	}

	factoryRst_powerCnt = 0;
	factoryRst_powerCntSave();

	factoryRst_timerEvt = NULL;
	return -1;
}

void factoryRst_handler(void){
	if(factoryRst_exist){
		factoryRst_exist = FALSE;
		zb_factoryReset();
	}
}

void factoryRst_init(void){
	factoryRst_powerCntRestore();
	factoryRst_powerCnt++;
	factoryRst_powerCntSave();

	if(factoryRst_timerEvt){
		TL_ZB_TIMER_CANCEL(&factoryRst_timerEvt);
	}
	factoryRst_timerEvt = TL_ZB_TIMER_SCHEDULE(factoryRst_timerCb, NULL, FACTORY_RESET_TIMEOUT * 1000);
}

