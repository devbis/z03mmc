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
#include "reporting.h"


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
extern u8 lcd_version;
extern u8 sensor_version;


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

	zcl_thermostatDisplayMode_restore();
	zcl_calibration_restore();
	zcl_reportingTabInit();

	/* Register ZCL specific cluster information */
	zcl_register(SENSOR_DEVICE_ENDPOINT, SENSOR_DEVICE_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_sensorDeviceClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&sensorDevice_simpleDesc, &sensorDevice_otaInfo, &sensorDevice_otaCb);
#endif

	show_zigbe();

    // read sensor every 10 seconds
	u16 sensorReadPeriod = DISPLAY_ON_SENSOR_READ_PERIOD;
	if (!g_zcl_thermostatUICfgAttrs.displayOn) {
		sensorReadPeriod = DISPLAY_OFF_SENSOR_READ_PERIOD;
	}
    read_sensor_start(sensorReadPeriod);
}

_attribute_ram_code_
u8 is_comfort(s16 t, u16 h) {
	u8 ret = 0;
	if (
		t >= g_zcl_thermostatUICfgAttrs.tempComfMin && t <= g_zcl_thermostatUICfgAttrs.tempComfMax &&
		h >= g_zcl_thermostatUICfgAttrs.humidComfMin && h <= g_zcl_thermostatUICfgAttrs.humidComfMax
	)
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
    g_zcl_temperatureAttrs.measuredValue = temp + g_zcl_temperatureAttrs.calibration;
#ifdef ZCL_RELATIVE_HUMIDITY
	s16 humiWithOffset = humi + g_zcl_relHumidityAttrs.calibration;
	if (humiWithOffset > 10000) {
		humiWithOffset = 10000;
	} else if (humiWithOffset < 0) {
		humiWithOffset = 0;
	}
    g_zcl_relHumidityAttrs.measuredValue = (u16)humiWithOffset;
#endif

    voltage = drv_get_adc_data();
    converted_voltage = (u8)(voltage / 100);
	percentage = ((voltage - BATTERY_SAFETY_THRESHOLD) / 4);
	if (percentage > 0xc8) percentage = 0xc8;
	percentage2 = (u8)percentage;

	// printf("converted voltage %d diff %d", converted_voltage, (voltage - BATTERY_SAFETY_THRESHOLD));
	//printf(" , percentage2 %d\r\n", percentage2);
    g_zcl_powerAttrs.batteryVoltage = converted_voltage;
    g_zcl_powerAttrs.batteryPercentage = percentage2;

	s16 displayTemperature = g_zcl_temperatureAttrs.measuredValue / 10;
	u8 tempSymbol = 1;
	u8 hasPoint = 1;

	if (g_zcl_thermostatUICfgAttrs.displayOn) {

#ifdef ZCL_THERMOSTAT_UI_CFG
		if (g_zcl_thermostatUICfgAttrs.displayMode == 1) {
			tempSymbol = 2;
			displayTemperature = (s16)(((s32)g_zcl_temperatureAttrs.measuredValue * 9) / (5*10) + 320);
			if (displayTemperature > 999) {
				hasPoint = 0;
				displayTemperature = displayTemperature / 10;
			}
		}
#endif

		// update lcd
		show_temp_symbol(tempSymbol);
		show_big_number(displayTemperature, hasPoint);
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
#ifdef ZCL_TEMPERATURE_MEASUREMENT
		show_small_number(g_zcl_relHumidityAttrs.measuredValue / 100, 1);
		show_battery_symbol(percentage <= 10);
		if (!g_zcl_thermostatUICfgAttrs.smileyOn) {
			show_smiley(0);
		} else {
			show_smiley(
				is_comfort(g_zcl_temperatureAttrs.measuredValue, g_zcl_relHumidityAttrs.measuredValue) ? 1 : 2
			);
		}
#endif
#endif
    	update_lcd();
	}
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
			// u16 second = 1;//TODO: fix me

			reportNoMinLimit();

			//start report timer
			app_reportAttrTimerStart();
			// reportAttrTimerStart(second);
		}else{
			//stop report timer
			// reportAttrTimerStop();
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
	zb_resetDevice();
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

void populate_sw_build() {
	g_zcl_basicAttrs.swBuildId[1] = int_to_hex(STACK_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[2] = int_to_hex(STACK_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[3] = int_to_hex(STACK_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[4] = int_to_hex(STACK_BUILD & 0xf);
	g_zcl_basicAttrs.swBuildId[6] = int_to_hex(APP_RELEASE>>4);
	g_zcl_basicAttrs.swBuildId[7] = int_to_hex(APP_RELEASE & 0xf);
	g_zcl_basicAttrs.swBuildId[8] = int_to_hex(APP_BUILD>>4);
	g_zcl_basicAttrs.swBuildId[9] = int_to_hex(APP_BUILD & 0xf);
}

void populate_date_code() {
	u8 month;
	if (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n') month = 1;
	else if (__DATE__[0] == 'F') month = 2;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r') month = 3;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'p') month = 4;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y') month = 5;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n') month = 6;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l') month = 7;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'u') month = 8;
	else if (__DATE__[0] == 'S') month = 9;
	else if (__DATE__[0] == 'O') month = 10;
	else if (__DATE__[0] == 'N') month = 11;
	else if (__DATE__[0] == 'D') month = 12;

	g_zcl_basicAttrs.dateCode[1] = __DATE__[7];
	g_zcl_basicAttrs.dateCode[2] = __DATE__[8];
	g_zcl_basicAttrs.dateCode[3] = __DATE__[9];
	g_zcl_basicAttrs.dateCode[4] = __DATE__[10];
	g_zcl_basicAttrs.dateCode[5] = '0' + month / 10;
	g_zcl_basicAttrs.dateCode[6] = '0' + month % 10;
	g_zcl_basicAttrs.dateCode[7] = __DATE__[4] >= '0' ? (__DATE__[4]) : '0';
	g_zcl_basicAttrs.dateCode[8] = __DATE__[5];
}


void populate_hw_version() {
	/*
	HW   | LCD I2C   addr | SHTxxx   I2C addr | Note
-----|----------------|-------------------|---------
B1.4 | 0x3C           | 0x70   (SHTC3)    |
B1.5 | UART!          | 0x70   (SHTC3)    |
B1.6 | UART!          | 0x44   (SHT4x)    |
B1.7 | 0x3C           | 0x44   (SHT4x)    | Test   original string HW
B1.9 | 0x3E           | 0x44   (SHT4x)    |
B2.0 | 0x3C           | 0x44   (SHT4x)    | Test   original string HW
	*/
    if (lcd_version == 0) {
        if (sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = 14;
        else if (sensor_version == 1)
            g_zcl_basicAttrs.hwVersion = 20;
    } else if (lcd_version == 1) {
        if (sensor_version == 0)
            g_zcl_basicAttrs.hwVersion = 15;
        else if (sensor_version == 1)
            g_zcl_basicAttrs.hwVersion = 16;
    } else if (lcd_version == 2) {
        g_zcl_basicAttrs.hwVersion = 19;
    }
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
	if(!isRetention){
	    /* Populate properties with compiled-in values */
		populate_sw_build();
		populate_date_code();

		/* Initialize Stack */
		stack_init();

		init_lcd(true);
		init_sensor();

		populate_hw_version();

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
		u8 batteryVoltageReportableChange = 0;
		u8 batteryPercentReportableChange = 0;
		u16 temperatureReportableChange = 10;
		u16 humidityReportableChange = 50;
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_VOLTAGE,
			60,
			3600,
			(u8 *)&batteryVoltageReportableChange
		);
        bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
			60,
			3600,
			(u8 *)&batteryPercentReportableChange
		);
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
			ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,
			10,
			300,
			(u8 *)&temperatureReportableChange
		);
		bdb_defaultReportingCfg(
			SENSOR_DEVICE_ENDPOINT,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
			ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,
			10,
			300,
			(u8 *)&humidityReportableChange
		);

		/* Initialize BDB */
		u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
		bdb_init((af_simple_descriptor_t *)&sensorDevice_simpleDesc, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);
	}else{
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
		init_lcd_deepsleep();
	}
}
