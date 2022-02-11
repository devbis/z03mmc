
#ifndef ZCL_RELATIVE_HUMIDITY_H
#define ZCL_RELATIVE_HUMIDITY_H



/*********************************************************************
 * CONSTANTS
 */

/**
 *  @brief	relative humidity measurement cluster Attribute IDs
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

extern const zclAttrInfo_t relative_humidity_measurement_attrTbl[];
extern const u8 zcl_relative_humidity_measurement_attrNum;
#endif


status_t zcl_relative_humidity_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif	/* ZCL_RELATIVE_HUMIDITY_H */
