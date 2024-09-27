
#ifndef ZCL_CONCENTRATION_H
#define ZCL_CONCENTRATION_H

/*********************************************************************
 * CONSTANTS
 */

#define ZCL_CLUSTER_CM_PM25				  0x042a
#define ZCL_CLUSTER_CM_PM1				  0x042c
#define ZCL_CLUSTER_CM_PM10				  0x042d


/**
 *  @brief	Particulate Matter 2.5 cluster Attribute IDs
 */
#define ZCL_CONCENTRATION_ATTRID_MEASUREDVALUE     0x0000
#define ZCL_CONCENTRATION_ATTRID_MINMEASUREDVALUE  0x0001
#define ZCL_CONCENTRATION_ATTRID_MAXMEASUREDVALUE  0x0002
#define ZCL_CONCENTRATION_ATTRID_TOLERANCE         0x0003

status_t zcl_pm25_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);
status_t zcl_pm1_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);
status_t zcl_pm10_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif	/* ZCL_CONCENTRATION_H */
