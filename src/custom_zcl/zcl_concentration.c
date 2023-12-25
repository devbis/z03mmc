
/**********************************************************************
 * INCLUDES
 */
#include "zcl_include.h"
#include "zcl_concentration.h"


#ifdef  ZCL_PM25

_CODE_ZCL_ status_t zcl_pm25_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_CM_PM25, manuCode, attrNum, attrTbl, NULL, cb);
}

#endif	/*  ZCL_PM25 */

#ifdef  ZCL_PM1

_CODE_ZCL_ status_t zcl_pm1_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_CM_PM1, manuCode, attrNum, attrTbl, NULL, cb);
}

#endif	/*  ZCL_PM1 */

#ifdef  ZCL_PM10

_CODE_ZCL_ status_t zcl_pm10_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_CM_PM10, manuCode, attrNum, attrTbl, NULL, cb);
}

#endif	/*  ZCL_PM10 */