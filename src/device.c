/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "device.h"
#include "app_ui.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif

#include "zcl_relative_humidity.h"
#include "app_i2c.h"
#include "shtv3_sensor.h"
#include "lcd.h"


/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
app_ctx_t g_sensorAppCtx;


#ifdef ZCL_OTA
extern ota_callBack_t sensorDevice_otaCb;

//running code firmware information
ota_preamble_t sensorDevice_otaInfo = {
	.fileVer 			= FILE_VERSION,
	.imageType 			= IMAGE_TYPE,
	.manufacturerCode 	= MANUFACTURER_CODE_TELINK,
};
#endif


//Must declare the application call back function which used by ZDO layer
const zdo_appIndCb_t appCbLst = {
	bdb_zdoStartDevCnf,//start device cnf cb
	NULL,//reset cnf cb
	NULL,//device announce indication cb
	sensorDevice_leaveIndHandler,//leave ind cb
	sensorDevice_leaveCnfHandler,//leave cnf cb
	NULL,//nwk update ind cb
	NULL,//permit join ind cb
	NULL,//nlme sync cnf cb
	NULL,//tc join ind cb
	NULL,//tc detects that the frame counter is near limit
};


/**
 *  @brief Definition for bdb commissioning setting
 */
bdb_commissionSetting_t g_bdbCommissionSetting = {
	.linkKey.tcLinkKey.keyType = SS_GLOBAL_LINK_KEY,
	.linkKey.tcLinkKey.key = (u8 *)tcLinkKeyCentralDefault,       		//can use unique link key stored in NV

	.linkKey.distributeLinkKey.keyType = MASTER_KEY,
	.linkKey.distributeLinkKey.key = (u8 *)linkKeyDistributedMaster,  	//use linkKeyDistributedCertification before testing

	.linkKey.touchLinkKey.keyType = MASTER_KEY,
	.linkKey.touchLinkKey.key = (u8 *)touchLinkKeyMaster,   			//use touchLinkKeyCertification before testing

#if TOUCHLINK_SUPPORT
	.touchlinkEnable = 1,												/* enable touch-link */
#else
	.touchlinkEnable = 0,												/* disable touch-link */
#endif
	.touchlinkChannel = DEFAULT_CHANNEL, 								/* touch-link default operation channel for target */
	.touchlinkLqiThreshold = 0xA0,			   							/* threshold for touch-link scan req/resp command */
};

#if PM_ENABLE
/**
 *  @brief Definition for wakeup source and level for PM
 */
drv_pm_pinCfg_t g_sensorPmCfg[] = {
	{
		BUTTON1,
		PM_WAKEUP_LEVEL
	},
	{
		BUTTON2,
		PM_WAKEUP_LEVEL
	}
};
#endif
/**********************************************************************
 * LOCAL VARIABLES
 */

//const scomfort_t def_cmf = {
_attribute_data_retention_ scomfort_t cmf = {
    .t = {2100,2600}, // x0.01 C
    .h = {3000,6000}  // x0.01 %
};


/**********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      stack_init
 *
 * @brief   This function initialize the ZigBee stack and related profile. If HA/ZLL profile is
 *          enabled in this application, related cluster should be registered here.
 *
 * @param   None
 *
 * @return  None
 */
void stack_init(void)
{
	zb_init();
	zb_zdoCbRegister((zdo_appIndCb_t *)&appCbLst);
}

/*********************************************************************
 * @fn      user_app_init
 *
 * @brief   This function initialize the application(Endpoint) information for this node.
 *
 * @param   None
 *
 * @return  None
 */
void user_app_init(void)
{
	// factoryRst_init();
#if ZCL_POLL_CTRL_SUPPORT
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_PERIODICALLY);
#else
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_WHEN_STIMULATED);
#endif
    /* Initialize ZCL layer */
	/* Register Incoming ZCL Foundation command/response messages */
	zcl_init(sensorDevice_zclProcessIncomingMsg);

	/* Register endPoint */
	af_endpointRegister(SENSOR_DEVICE_ENDPOINT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, zcl_rx_handler, NULL);

	zcl_reportingTabInit();

	/* Register ZCL specific cluster information */
	zcl_register(SENSOR_DEVICE_ENDPOINT, SENSOR_DEVICE_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_sensorDeviceClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, &sensorDevice_otaInfo, &sensorDevice_otaCb);
#endif

	show_zigbe();

    // read sensor every 10 seconds
    read_sensor_start(10000);
}

_attribute_ram_code_
u8 is_comfort(s16 t, u16 h) {
	u8 ret = 0;
	if (t >= cmf.t[0] && t <= cmf.t[1] && h >= cmf.h[0] && h <= cmf.h[1])
		ret = 1;
	return ret;
}

void read_sensor_and_save() {
	s16 temp = 0;
	u16 humi = 0;
    u16 voltage, percentage;
	u8 converted_voltage, percentage2;

	read_sensor(&temp,&humi);
    // printf("Temp: %d.%d, humid: %d\r\n", temp/10, temp % 10, humi);
    g_zcl_temperatureAttrs.measuredValue = temp * 10;
    g_zcl_relHumidityAttrs.measuredValue = humi * 100;

    voltage = drv_get_adc_data();
    converted_voltage = (u8)(voltage / 100);
	percentage = ((voltage - BATTERY_SAFETY_THRESHOLD) / 4);
	if (percentage > 0xc8) percentage = 0xc8;
	percentage2 = (u8)percentage;

	// printf("converted voltage %d diff %d", converted_voltage, (voltage - BATTERY_SAFETY_THRESHOLD));
	//printf(" , percentage2 %d\r\n", percentage2);
    g_zcl_powerAttrs.batteryVoltage = converted_voltage;
    g_zcl_powerAttrs.batteryPercentage = percentage2;

    // update lcd
    show_temp_symbol(1);
    show_big_number(g_zcl_temperatureAttrs.measuredValue / 10, 1);
    show_small_number(g_zcl_relHumidityAttrs.measuredValue / 100, 1);
#if defined(SHOW_SMILEY)
    show_smiley(
        is_comfort(g_zcl_temperatureAttrs.measuredValue, g_zcl_relHumidityAttrs.measuredValue) ? 1 : 2
    );
#endif
    update_lcd();
}

s32 zclSensorTimerCb(void *arg)
{
	u32 interval = g_sensorAppCtx.readSensorTime;
	read_sensor_and_save();
	return interval;
}

void read_sensor_start(u16 delayTime)
{
	u32 interval = 0;

	if(!g_sensorAppCtx.timerReadSensorEvt){
		read_sensor_and_save();
		interval = delayTime;
		g_sensorAppCtx.readSensorTime = delayTime;

		g_sensorAppCtx.timerReadSensorEvt = TL_ZB_TIMER_SCHEDULE(zclSensorTimerCb, NULL, interval);
	}
}

void ind_init(void)
{
	light_init();
}

void report_handler(void)
{
	if(zb_isDeviceJoinedNwk()){
		if(zcl_reportingEntryActiveNumGet()){
			u16 second = 1;//TODO: fix me

			reportNoMinLimit();

			//start report timer
			reportAttrTimerStart(second);
		}else{
			//stop report timer
			reportAttrTimerStop();
		}
	}
}

void app_task(void)
{
	app_key_handler();

	if(bdb_isIdle()){
#if PM_ENABLE
		if(!g_sensorAppCtx.keyPressed){
			drv_pm_lowPowerEnter();
		}
#endif
        // factoryRst_handler();
		report_handler();
	}
}

static void sensorDeviceSysException(void)
{
#if 1
	SYSTEM_RESET();
#else
	light_on();
	while(1);
#endif
}

char int_to_hex(u8 num){
	char digits[] = "0123456789ABCDEF";
	if (num > 15) return digits[0];
	return digits[num];
}


/*********************************************************************
 * @fn      user_init
 *
 * @brief   User level initialization code.
 *
 * @param   isRetention - if it is waking up with ram retention.
 *
 * @return  None
 */
void user_init(bool isRetention)
{
#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

#if ZBHCI_EN
	zbhciInit();
#endif

#if PM_ENABLE
	drv_pm_wakeupPinConfig(g_sensorPmCfg, sizeof(g_sensorPmCfg)/sizeof(drv_pm_pinCfg_t));
#endif

	init_i2c();
	init_sensor();
    init_lcd(!isRetention);
    // initialize indicator (ble symbol)
    ind_init();

	if(!isRetention){
	    /* Populate swBuildId version */
		g_zcl_basicAttrs.swBuildId[1] = int_to_hex(STACK_RELEASE>>4);
		g_zcl_basicAttrs.swBuildId[2] = int_to_hex(STACK_RELEASE & 0xf);
		g_zcl_basicAttrs.swBuildId[3] = int_to_hex(STACK_BUILD>>4);
		g_zcl_basicAttrs.swBuildId[4] = int_to_hex(STACK_BUILD & 0xf);
		g_zcl_basicAttrs.swBuildId[6] = int_to_hex(APP_RELEASE>>4);
		g_zcl_basicAttrs.swBuildId[7] = int_to_hex(APP_RELEASE & 0xf);
		g_zcl_basicAttrs.swBuildId[8] = int_to_hex(APP_BUILD>>4);
		g_zcl_basicAttrs.swBuildId[9] = int_to_hex(APP_BUILD & 0xf);

		/* Initialize Stack */
		stack_init();

		/* Initialize user application */
		user_app_init();

		/* Register except handler for test */
		sys_exceptHandlerRegister(sensorDeviceSysException);

		/* User's Task */
#if ZBHCI_EN
		ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
		ev_on_poll(EV_POLL_IDLE, app_task);

		/* Load the pre-install code from flash */
		if(bdb_preInstallCodeLoad(&g_sensorAppCtx.tcLinkKey.keyType, g_sensorAppCtx.tcLinkKey.key) == RET_OK){
			g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_sensorAppCtx.tcLinkKey.keyType;
			g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_sensorAppCtx.tcLinkKey.key;
		}

		/* Set default reporting configuration */
		u8 reportableChange = 0x00;
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_VOLTAGE,
			0x0000,
			3600,
			(u8 *)&reportableChange
		);
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
			0x0000,
			3600,
			(u8 *)&reportableChange
		);
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
			ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,
			0x0000,
			0x003c,
			(u8 *)&reportableChange
		);
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
			ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,
			0x0000,
			0x003c,
			(u8 *)&reportableChange
		);

		/* Initialize BDB */
		u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
		bdb_init((af_simple_descriptor_t *)&sensorDevice_simpleDesc, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);
	}else{
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
	}
}
