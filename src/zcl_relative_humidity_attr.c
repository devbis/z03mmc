/********************************************************************************************************
 * @file    zcl_temperature_measurement_attr.c
 *
 * @brief   This is the source file for zcl_temperature_measurement_attr
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

#if 0

/**********************************************************************
 * INCLUDES
 */
#include "../zcl_include.h"


/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * LOCAL TYPES
 */


/**********************************************************************
 * LOCAL VARIABLES
 */
#ifdef ZCL_TEMPERATURE_MEASUREMENT

#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MEASUREDVALUE_ENABLE
s16 zcl_attr_MeasuredValue = 0xFFFF;
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MINMEASUREDVALUE_ENABLE
s16 zcl_attr_MinMeasuredValue = 0x8000;
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MAXMEASUREDVALUE_ENABLE
s16 zcl_attr_MaxMeasuredValue = 0x8000;
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_TOLERANCE_ENABLE
u16 zcl_attr_Tolerance = 0;
#endif

const zclAttrInfo_t temperature_measurement_attrTbl[] =
{
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MEASUREDVALUE_ENABLE
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&zcl_attr_MeasuredValue},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MINMEASUREDVALUE_ENABLE
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&zcl_attr_MinMeasuredValue},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_MAXMEASUREDVALUE_ENABLE
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&zcl_attr_MaxMeasuredValue},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT_ATTR_TOLERANCE_ENABLE
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,           ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ, (u8*)&zcl_attr_Tolerance},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 					  ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ,  (u8*)&zcl_attr_global_clusterRevision},
};

const u8 zcl_temperature_measurement_attrNum = ( sizeof(temperature_measurement_attrTbl) / sizeof(zclAttrInfo_t) );

#endif	/* ZCL_TEMPERATURE_MEASUREMENT */

#endif
