
/**********************************************************************
 * INCLUDES
 */
#include "zcl_include.h"


#ifdef  ZCL_THERMOSTAT_UI_CFG
/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * LOCAL TYPES
 */


/**********************************************************************
 * LOCAL VARIABLES
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */

_CODE_ZCL_ status_t zcl_thermostat_ui_cfg_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_HAVC_USER_INTERFACE_CONFIG, manuCode, attrNum, attrTbl, NULL, cb);
}

#endif	/*  ZCL_THERMOSTAT_UI */

