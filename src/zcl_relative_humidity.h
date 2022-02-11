/********************************************************************************************************
 * @file    zcl_temperature_measurement.h
 *
 * @brief   This is the header file for zcl_temperature_measurement
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

#ifndef ZCL_RELATIVE_HUMIDITY_H
#define ZCL_RELATIVE_HUMIDITY_H



/*********************************************************************
 * CONSTANTS
 */

/**
 *  @brief	temperature measurement cluster Attribute IDs
 */
#define ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE     0x0000
#define ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE  0x0001
#define ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE  0x0002
#define ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE         0x0003


#if 0
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MEASUREDVALUE_ENABLE
extern s16 zcl_attr_MeasuredValue;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MINMEASUREDVALUE_ENABLE
extern s16 zcl_attr_MinMeasuredValue;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MAXMEASUREDVALUE_ENABLE
extern s16 zcl_attr_MaxMeasuredValue;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_TOLERANCE_ENABLE
extern u16 zcl_attr_Tolerance;
#endif

extern const zclAttrInfo_t temperature_measurement_attrTbl[];
extern const u8 zcl_temperature_measurement_attrNum;
#endif


status_t zcl_relative_humidity_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif	/* ZCL_RELATIVE_HUMIDITY_H */
