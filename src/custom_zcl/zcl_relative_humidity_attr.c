
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
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT

#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MEASUREDVALUE_ENABLE
u16 zcl_attr_MeasuredValue = 0xFFFF;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MINMEASUREDVALUE_ENABLE
u16 zcl_attr_MinMeasuredValue = 0x8000;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MAXMEASUREDVALUE_ENABLE
u16 zcl_attr_MaxMeasuredValue = 0x8000;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_TOLERANCE_ENABLE
u16 zcl_attr_Tolerance = 0;
#endif

const zclAttrInfo_t relative_humidity_measurement_attrTbl[] =
{
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MEASUREDVALUE_ENABLE
	{ ZCL_RELATIVE_HUMIDITY_MEASUREMENT_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&zcl_attr_MeasuredValue},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MINMEASUREDVALUE_ENABLE
	{ ZCL_RELATIVE_HUMIDITY_MEASUREMENT_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&zcl_attr_MinMeasuredValue},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_MAXMEASUREDVALUE_ENABLE
	{ ZCL_RELATIVE_HUMIDITY_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&zcl_attr_MaxMeasuredValue},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_ATTR_TOLERANCE_ENABLE
	{ ZCL_RELATIVE_HUMIDITY_MEASUREMENT_ATTRID_TOLERANCE,           ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ, (u8*)&zcl_attr_Tolerance},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 					  ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ,  (u8*)&zcl_attr_global_clusterRevision},
};

const u8 zcl_relative_humidity_measurement_attrNum = ( sizeof(relative_humidity_measurement_attrTbl) / sizeof(zclAttrInfo_t) );

#endif	/* ZCL_RELATIVE_HUMIDITY_MEASUREMENT */

#endif
