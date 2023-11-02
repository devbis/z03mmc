
#ifndef ZCL_THERMOSTAT_UI_CFG_H
#define ZCL_THERMOSTAT_UI_CFG_H

/*********************************************************************
 * CONSTANTS
 */

/**
 *  @brief	thermostatu UI configuration cluster Attribute IDs
 */
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_TEMPERATUREDISPLAYMODE         0x0000
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_KEYPADLOCKOUT                  0x0001
#define ZCL_THERMOSTAT_UI_CFG_ATTRID_SCHEDULEPROGRAMMINGVISIBILITY  0x0002

status_t zcl_thermostat_ui_cfg_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif	/* ZCL_THERMOSTAT_UI_CFG_H */
