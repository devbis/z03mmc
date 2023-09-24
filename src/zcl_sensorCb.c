/********************************************************************************************************
 * @file    zcl_sampleSensorCb.c
 *
 * @brief   This is the source file for zcl_sampleSensorCb
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


/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "device.h"
#include "app_ui.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
#ifdef ZCL_READ
static void sampleSensor_zclReadRspCmd(u16 clusterId, zclReadRspCmd_t *pReadRspCmd);
#endif
#ifdef ZCL_WRITE
static void sampleSensor_zclWriteRspCmd(u16 clusterId, zclWriteRspCmd_t *pWriteRspCmd);
static void sampleSensor_zclWriteReqCmd(u16 clusterId, zclWriteCmd_t *pWriteReqCmd);
#endif
#ifdef ZCL_REPORT
static void sampleSensor_zclCfgReportCmd(u16 clusterId, zclCfgReportCmd_t *pCfgReportCmd);
static void sampleSensor_zclCfgReportRspCmd(u16 clusterId, zclCfgReportRspCmd_t *pCfgReportRspCmd);
static void sampleSensor_zclReportCmd(u16 clusterId, zclReportCmd_t *pReportCmd);
#endif
static void sampleSensor_zclDfltRspCmd(u16 clusterId, zclDefaultRspCmd_t *pDftRspCmd);

/**********************************************************************
 * GLOBAL VARIABLES
 */

/**********************************************************************
 * LOCAL VARIABLES
 */
#ifdef ZCL_IDENTIFY
static ev_timer_event_t *identifyTimerEvt = NULL;
#endif

/**********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      sampleSensor_zclProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message.
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  None
 */
void sampleSensor_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg)
{
	printf("sampleSensor_zclProcessIncomingMsg %d\n", pInHdlrMsg->hdr.cmd);
	u16 cluster = pInHdlrMsg->msg->indInfo.cluster_id;
	switch(pInHdlrMsg->hdr.cmd)
	{
#ifdef ZCL_READ
		case ZCL_CMD_READ_RSP:
			sampleSensor_zclReadRspCmd(cluster, pInHdlrMsg->attrCmd);
			break;
#endif
#ifdef ZCL_WRITE
		case ZCL_CMD_WRITE_RSP:
			sampleSensor_zclWriteRspCmd(cluster, pInHdlrMsg->attrCmd);
			break;
		case ZCL_CMD_WRITE:
			sampleSensor_zclWriteReqCmd(cluster, pInHdlrMsg->attrCmd);
			break;
#endif
#ifdef ZCL_REPORT
		case ZCL_CMD_CONFIG_REPORT:
			sampleSensor_zclCfgReportCmd(cluster, pInHdlrMsg->attrCmd);
			break;
		case ZCL_CMD_CONFIG_REPORT_RSP:
			sampleSensor_zclCfgReportRspCmd(cluster, pInHdlrMsg->attrCmd);
			break;
		case ZCL_CMD_REPORT:
			sampleSensor_zclReportCmd(cluster, pInHdlrMsg->attrCmd);
			break;
#endif
		case ZCL_CMD_DEFAULT_RSP:
			sampleSensor_zclDfltRspCmd(cluster, pInHdlrMsg->attrCmd);
			break;
		default:
			break;
	}
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      sampleSensor_zclReadRspCmd
 *
 * @brief   Handler for ZCL Read Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclReadRspCmd(u16 clusterId, zclReadRspCmd_t *pReadRspCmd)
{
    printf("sampleSensor_zclReadRspCmd\n");

}
#endif	/* ZCL_READ */

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      sampleSensor_zclWriteRspCmd
 *
 * @brief   Handler for ZCL Write Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclWriteRspCmd(u16 clusterId, zclWriteRspCmd_t *pWriteRspCmd)
{
    printf("sampleSensor_zclWriteRspCmd\n");

}

/*********************************************************************
 * @fn      sampleSensor_zclWriteReqCmd
 *
 * @brief   Handler for ZCL Write Request command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclWriteReqCmd(u16 clusterId, zclWriteCmd_t *pWriteReqCmd)
{
#ifdef ZCL_POLL_CTRL
	u8 numAttr = pWriteReqCmd->numAttr;
	zclWriteRec_t *attr = pWriteReqCmd->attrList;

	if(clusterId == ZCL_CLUSTER_GEN_POLL_CONTROL){
		for(s32 i = 0; i < numAttr; i++){
			if(attr[i].attrID == ZCL_ATTRID_CHK_IN_INTERVAL){
				sampleSensor_zclCheckInStart();
				return;
			}
		}
	}
#endif
}
#endif	/* ZCL_WRITE */


/*********************************************************************
 * @fn      sampleSensor_zclDfltRspCmd
 *
 * @brief   Handler for ZCL Default Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclDfltRspCmd(u16 clusterId, zclDefaultRspCmd_t *pDftRspCmd)
{
    printf("sampleSensor_zclDfltRspCmd\n");

}

#ifdef ZCL_REPORT
/*********************************************************************
 * @fn      sampleSensor_zclCfgReportCmd
 *
 * @brief   Handler for ZCL Configure Report command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclCfgReportCmd(u16 clusterId, zclCfgReportCmd_t *pCfgReportCmd)
{
    printf("sampleSensor_zclCfgReportCmd\n");

}

/*********************************************************************
 * @fn      sampleSensor_zclCfgReportRspCmd
 *
 * @brief   Handler for ZCL Configure Report Response command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclCfgReportRspCmd(u16 clusterId, zclCfgReportRspCmd_t *pCfgReportRspCmd)
{
    printf("sampleSensor_zclCfgReportRspCmd\n");

}

/*********************************************************************
 * @fn      sampleSensor_zclReportCmd
 *
 * @brief   Handler for ZCL Report command.
 *
 * @param   pInHdlrMsg - incoming message to process
 *
 * @return  None
 */
static void sampleSensor_zclReportCmd(u16 clusterId, zclReportCmd_t *pReportCmd)
{
    printf("sampleSensor_zclReportCmd\n");

}
#endif	/* ZCL_REPORT */

#ifdef ZCL_BASIC
/*********************************************************************
 * @fn      sampleSensor_basicCb
 *
 * @brief   Handler for ZCL Basic Reset command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sampleSensor_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
    printf("sampleSensor_basicCb\n");
	if(cmdId == ZCL_CMD_BASIC_RESET_FAC_DEFAULT){
		//Reset all the attributes of all its clusters to factory defaults
		//zcl_nv_attr_reset();
	}

	return ZCL_STA_SUCCESS;
}
#endif	/* ZCL_BASIC */

#ifdef ZCL_IDENTIFY
s32 sampleSensor_zclIdentifyTimerCb(void *arg)
{
    printf("sampleSensor_zclIdentifyTimerCb\n");
	if(g_zcl_identifyAttrs.identifyTime <= 0){
		identifyTimerEvt = NULL;
		return -1;
	}
	g_zcl_identifyAttrs.identifyTime--;
	return 0;
}

void sampleSensor_zclIdentifyTimerStop(void)
{
    printf("sampleSensor_zclIdentifyTimerStop\n");
	if(identifyTimerEvt){
		TL_ZB_TIMER_CANCEL(&identifyTimerEvt);
	}
}

/*********************************************************************
 * @fn      sampleSensor_zclIdentifyCmdHandler
 *
 * @brief   Handler for ZCL Identify command. This function will set blink LED.
 *
 * @param	endpoint
 * @param	srcAddr
 * @param   identifyTime - identify time
 *
 * @return  None
 */
void sampleSensor_zclIdentifyCmdHandler(u8 endpoint, u16 srcAddr, u16 identifyTime)
{
	g_zcl_identifyAttrs.identifyTime = identifyTime;

	if(identifyTime == 0){
		sampleSensor_zclIdentifyTimerStop();
		light_blink_stop();
	}else{
		if(!identifyTimerEvt){
			light_blink_start(identifyTime, 500, 500);
			identifyTimerEvt = TL_ZB_TIMER_SCHEDULE(sampleSensor_zclIdentifyTimerCb, NULL, 1000);
		}
	}
}

/*********************************************************************
 * @fn      sampleSensor_zcltriggerCmdHandler
 *
 * @brief   Handler for ZCL trigger command.
 *
 * @param   pTriggerEffect
 *
 * @return  None
 */
static void sampleSensor_zcltriggerCmdHandler(zcl_triggerEffect_t *pTriggerEffect)
{
	u8 effectId = pTriggerEffect->effectId;
	//u8 effectVariant = pTriggerEffect->effectVariant;

	switch(effectId){
		case IDENTIFY_EFFECT_BLINK:
			light_blink_start(1, 500, 500);
			break;
		case IDENTIFY_EFFECT_BREATHE:
			light_blink_start(15, 300, 700);
			break;
		case IDENTIFY_EFFECT_OKAY:
			light_blink_start(2, 250, 250);
			break;
		case IDENTIFY_EFFECT_CHANNEL_CHANGE:
			light_blink_start(1, 500, 7500);
			break;
		case IDENTIFY_EFFECT_FINISH_EFFECT:
			light_blink_start(1, 300, 700);
			break;
		case IDENTIFY_EFFECT_STOP_EFFECT:
			light_blink_stop();
			break;
		default:
			break;
	}
}

/*********************************************************************
 * @fn      sampleSensor_zclIdentifyQueryRspCmdHandler
 *
 * @brief   Handler for ZCL Identify Query response command.
 *
 * @param   endpoint
 * @param   srcAddr
 * @param   identifyRsp
 *
 * @return  None
 */
static void sampleSensor_zclIdentifyQueryRspCmdHandler(u8 endpoint, u16 srcAddr, zcl_identifyRspCmd_t *identifyRsp)
{
#if FIND_AND_BIND_SUPPORT
	if(identifyRsp->timeout){
		findBindDst_t dstInfo;
		dstInfo.addr = srcAddr;
		dstInfo.endpoint = endpoint;

		bdb_addIdentifyActiveEpForFB(dstInfo);
	}
#endif
}

/*********************************************************************
 * @fn      sampleSensor_identifyCb
 *
 * @brief   Handler for ZCL Identify command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sampleSensor_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	if(pAddrInfo->dstEp == SAMPLE_SENSOR_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_IDENTIFY:
					sampleSensor_zclIdentifyCmdHandler(pAddrInfo->dstEp, pAddrInfo->srcAddr, ((zcl_identifyCmd_t *)cmdPayload)->identifyTime);
					break;
				case ZCL_CMD_TRIGGER_EFFECT:
					sampleSensor_zcltriggerCmdHandler((zcl_triggerEffect_t *)cmdPayload);
					break;
				default:
					break;
			}
		}else{
			if(cmdId == ZCL_CMD_IDENTIFY_QUERY_RSP){
				sampleSensor_zclIdentifyQueryRspCmdHandler(pAddrInfo->dstEp, pAddrInfo->srcAddr, (zcl_identifyRspCmd_t *)cmdPayload);
			}
		}
	}

	return ZCL_STA_SUCCESS;
}
#endif	/* ZCL_IDENTIFY */

#ifdef ZCL_IAS_ZONE
/*********************************************************************
 * @fn      sampleSensor_zclIasZoneEnrollRspCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE Enroll response command.
 *
 * @param   pZoneEnrollRsp
 *
 * @return  None
 */
static void sampleSensor_zclIasZoneEnrollRspCmdHandler(zoneEnrollRsp_t *pZoneEnrollRsp)
{

}

/*********************************************************************
 * @fn      sampleSensor_zclIasZoneInitNormalOperationModeCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE normal operation mode command.
 *
 * @param
 *
 * @return  status
 */
static status_t sampleSensor_zclIasZoneInitNormalOperationModeCmdHandler(void)
{
	u8 status = ZCL_STA_FAILURE;

	return status;
}

/*********************************************************************
 * @fn      sampleSensor_zclIasZoneInitTestModeCmdHandler
 *
 * @brief   Handler for ZCL IAS ZONE test mode command.
 *
 * @param   pZoneInitTestMode
 *
 * @return  status
 */
static status_t sampleSensor_zclIasZoneInitTestModeCmdHandler(zoneInitTestMode_t *pZoneInitTestMode)
{
	u8 status = ZCL_STA_FAILURE;

	return status;
}

/*********************************************************************
 * @fn      sampleSensor_iasZoneCb
 *
 * @brief   Handler for ZCL IAS Zone command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sampleSensor_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	status_t status = ZCL_STA_SUCCESS;

	if(pAddrInfo->dstEp == SAMPLE_SENSOR_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_ZONE_ENROLL_RSP:
					sampleSensor_zclIasZoneEnrollRspCmdHandler((zoneEnrollRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_INIT_NORMAL_OPERATION_MODE:
					sampleSensor_zclIasZoneInitNormalOperationModeCmdHandler();
					break;
				case ZCL_CMD_INIT_TEST_MODE:
					sampleSensor_zclIasZoneInitTestModeCmdHandler((zoneInitTestMode_t *)cmdPayload);
					break;
				default:
					break;
			}
		}
	}

	return status;
}
#endif  /* ZCL_IAS_ZONE */

#ifdef ZCL_POLL_CTRL
static ev_timer_event_t *zclFastPollTimeoutTimerEvt = NULL;
static ev_timer_event_t *zclCheckInTimerEvt = NULL;
static bool isFastPollMode = FALSE;

void sampleSensor_zclCheckInCmdSend(void)
{
	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);

	dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
	dstEpInfo.dstEp = SAMPLE_SENSOR_ENDPOINT;
	dstEpInfo.profileId = HA_PROFILE_ID;

	zcl_pollCtrl_checkInCmd(SAMPLE_SENSOR_ENDPOINT, &dstEpInfo, TRUE);
}

s32 sampleSensor_zclCheckInTimerCb(void *arg)
{
	static u32 cnt = 0;
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if(!pPollCtrlAttr->chkInInterval){
		cnt = 0;
		zclCheckInTimerEvt = NULL;
		return -1;
	}

	if(++cnt >= pPollCtrlAttr->chkInInterval){
		cnt = 0;
		sampleSensor_zclCheckInCmdSend();
	}

	return 0;
}

void sampleSensor_zclCheckInStart(void)
{
	if(zb_bindingTblSearched(ZCL_CLUSTER_GEN_POLL_CONTROL, SAMPLE_SENSOR_ENDPOINT)){
		zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

		if(!zclCheckInTimerEvt){
			zclCheckInTimerEvt = TL_ZB_TIMER_SCHEDULE(sampleSensor_zclCheckInTimerCb, NULL, POLL_RATE_QUARTERSECONDS);

			if(pPollCtrlAttr->chkInInterval){
				sampleSensor_zclCheckInCmdSend();
			}
		}
	}
}

void sampleSensor_zclSetFastPollMode(bool fastPollMode)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	isFastPollMode = fastPollMode;
	u32 pollRate = fastPollMode ? pPollCtrlAttr->shortPollInterval : pPollCtrlAttr->longPollInterval;

	zb_setPollRate(pollRate * QUEUE_POLL_RATE);
}

s32 sampleSensor_zclFastPollTimeoutCb(void *arg)
{
	static u32 cnt = 0;
	u32 fastPollTimeoutCnt = (u32)arg;

	if(++cnt >= fastPollTimeoutCnt){
		cnt = 0;

		sampleSensor_zclSetFastPollMode(FALSE);

		zclFastPollTimeoutTimerEvt = NULL;
		return -1;
	}else{
		return 0;
	}
}

static status_t sampleSensor_zclPollCtrlChkInRspCmdHandler(zcl_chkInRsp_t *pCmd)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if(pCmd->startFastPolling){
		u32 fastPollTimeoutCnt = 0;

		if(pCmd->fastPollTimeout){
			if(pCmd->fastPollTimeout > pPollCtrlAttr->fastPollTimeoutMax){
				return ZCL_STA_INVALID_FIELD;
			}

			fastPollTimeoutCnt = pCmd->fastPollTimeout;

			if(zclFastPollTimeoutTimerEvt){
				TL_ZB_TIMER_CANCEL(&zclFastPollTimeoutTimerEvt);
			}
		}else{
			if(!zclFastPollTimeoutTimerEvt){
				fastPollTimeoutCnt = pPollCtrlAttr->fastPollTimeout;
			}
		}

		if(!zclFastPollTimeoutTimerEvt && fastPollTimeoutCnt){
			sampleSensor_zclSetFastPollMode(TRUE);

			zclFastPollTimeoutTimerEvt = TL_ZB_TIMER_SCHEDULE(sampleSensor_zclFastPollTimeoutCb, (void *)fastPollTimeoutCnt, POLL_RATE_QUARTERSECONDS);
		}
	}else{
		//continue in normal operation and not required to go into fast poll mode.
	}

	return ZCL_STA_SUCCESS;
}

static status_t sampleSensor_zclPollCtrlFastPollStopCmdHandler(void)
{
	if(!isFastPollMode){
		return ZCL_STA_ACTION_DENIED;
	}else{
		if(zclFastPollTimeoutTimerEvt){
			TL_ZB_TIMER_CANCEL(&zclFastPollTimeoutTimerEvt);
		}
		sampleSensor_zclSetFastPollMode(FALSE);
	}

	return ZCL_STA_SUCCESS;
}

static status_t sampleSensor_zclPollCtrlSetLongPollIntervalCmdHandler(zcl_setLongPollInterval_t *pCmd)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if((pCmd->newLongPollInterval >= 0x04) && (pCmd->newLongPollInterval <= 0x6E0000)
		&& (pCmd->newLongPollInterval <= pPollCtrlAttr->chkInInterval) && (pCmd->newLongPollInterval >= pPollCtrlAttr->shortPollInterval)){
		pPollCtrlAttr->longPollInterval = pCmd->newLongPollInterval;
	}else{
		return ZCL_STA_INVALID_VALUE;
	}

	return ZCL_STA_SUCCESS;
}

static status_t sampleSensor_zclPollCtrlSetShortPollIntervalCmdHandler(zcl_setShortPollInterval_t *pCmd)
{
	zcl_pollCtrlAttr_t *pPollCtrlAttr = zcl_pollCtrlAttrGet();

	if((pCmd->newShortPollInterval >= 0x01) && (pCmd->newShortPollInterval <= 0xff)
		&& (pCmd->newShortPollInterval <= pPollCtrlAttr->longPollInterval)){
		pPollCtrlAttr->shortPollInterval = pCmd->newShortPollInterval;
	}else{
		return ZCL_STA_INVALID_VALUE;
	}

	return ZCL_STA_SUCCESS;
}

/*********************************************************************
 * @fn      sampleSensor_pollCtrlCb
 *
 * @brief   Handler for ZCL Poll Control command.
 *
 * @param   pAddrInfo
 * @param   cmdId
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t sampleSensor_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	status_t status = ZCL_STA_SUCCESS;

	if(pAddrInfo->dstEp == SAMPLE_SENSOR_ENDPOINT){
		if(pAddrInfo->dirCluster == ZCL_FRAME_CLIENT_SERVER_DIR){
			switch(cmdId){
				case ZCL_CMD_CHK_IN_RSP:
					status = sampleSensor_zclPollCtrlChkInRspCmdHandler((zcl_chkInRsp_t *)cmdPayload);
					break;
				case ZCL_CMD_FAST_POLL_STOP:
					status = sampleSensor_zclPollCtrlFastPollStopCmdHandler();
					break;
				case ZCL_CMD_SET_LONG_POLL_INTERVAL:
					status = sampleSensor_zclPollCtrlSetLongPollIntervalCmdHandler((zcl_setLongPollInterval_t *)cmdPayload);
					break;
				case ZCL_CMD_SET_SHORT_POLL_INTERVAL:
					status = sampleSensor_zclPollCtrlSetShortPollIntervalCmdHandler((zcl_setShortPollInterval_t *)cmdPayload);
					break;
				default:
					break;
			}
		}
	}

	return status;
}
#endif	/* ZCL_POLL_CTRL */
