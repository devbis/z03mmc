################################
# Common settings

SET(TARGET z03mmc)

IF(NOT MIMIC_HEIMAN)
    SET(MIMIC_HEIMAN 0)
ELSE()
    SET(TARGET ${TARGET}_heiman)
ENDIF()



ADD_DEFINITIONS(
    -DEND_DEVICE=1
    -DMCU_CORE_${TELINK_PLATFORM}=1
    -DMCU_STARTUP_${TELINK_PLATFORM}=1
    -DMIMIC_HEIMAN=${MIMIC_HEIMAN}
)

################################
# Zigbee Library

SET(ZIGBEE_SRC
    ${SDK_PREFIX}/platform/boot/link_cfg.S
    ${SDK_PREFIX}/platform/boot/${TELINK_PLATFORM}/cstartup_${TELINK_PLATFORM}.S
    ${SDK_PREFIX}/platform/services/b85m/irq_handler.c
    ${SDK_PREFIX}/platform/tc32/div_mod.S
    ${SDK_PREFIX}/platform/chip_${TELINK_PLATFORM}/flash.c
    ${SDK_PREFIX}/platform/chip_${TELINK_PLATFORM}/adc.c
    ${SDK_PREFIX}/platform/chip_${TELINK_PLATFORM}/flash/flash_mid13325e.c

    ${SDK_PREFIX}/zigbee/bdb/bdb.c
    ${SDK_PREFIX}/zigbee/aps/aps_group.c
    ${SDK_PREFIX}/zigbee/mac/mac_phy.c
    ${SDK_PREFIX}/zigbee/mac/mac_pib.c
    ${SDK_PREFIX}/zigbee/zdo/zdp.c
    ${SDK_PREFIX}/zigbee/zcl/zcl.c
    ${SDK_PREFIX}/zigbee/zcl/zcl_nv.c
    ${SDK_PREFIX}/zigbee/zcl/zcl_reporting.c
    ${SDK_PREFIX}/zigbee/zcl/smart_energy/zcl_metering.c
    ${SDK_PREFIX}/zigbee/zcl/smart_energy/zcl_metering_attr.c
    ${SDK_PREFIX}/zigbee/zcl/zll_commissioning/zcl_toucklink_security.c
    ${SDK_PREFIX}/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkDiscovery.c
    ${SDK_PREFIX}/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkJoinOrStart.c
    ${SDK_PREFIX}/zigbee/zcl/zll_commissioning/zcl_zll_commissioning.c
    ${SDK_PREFIX}/zigbee/zcl/commissioning/zcl_commissioning.c
    ${SDK_PREFIX}/zigbee/zcl/commissioning/zcl_commissioning_attr.c
    ${SDK_PREFIX}/zigbee/zcl/hvac/zcl_thermostat.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_electrical_measurement.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_electrical_measurement_attr.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_illuminance_measurement.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_illuminance_measurement_attr.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_occupancy_sensing.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_occupancy_sensing_attr.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_temperature_measurement.c
    ${SDK_PREFIX}/zigbee/zcl/measument_sensing/zcl_temperature_measurement_attr.c
    ${SDK_PREFIX}/zigbee/zcl/light_color_control/zcl_light_colorCtrl.c
    ${SDK_PREFIX}/zigbee/zcl/light_color_control/zcl_light_colorCtrl_attr.c
    ${SDK_PREFIX}/zigbee/zcl/closures/zcl_door_lock.c
    ${SDK_PREFIX}/zigbee/zcl/closures/zcl_door_lock_attr.c
    ${SDK_PREFIX}/zigbee/zcl/closures/zcl_window_covering.c
    ${SDK_PREFIX}/zigbee/zcl/closures/zcl_window_covering_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_alarm.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_alarm_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_basic.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_basic_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_binary_input.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_binary_input_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_binary_output.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_binary_output_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_devTemperatureCfg.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_devTemperatureCfg_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_diagnostics.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_diagnostics_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_greenPower.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_greenPower_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_group.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_group_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_identify.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_identify_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_level.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_level_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_multistate_input.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_multistate_input_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_multistate_output.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_multistate_output_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_onoff.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_onoff_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_pollCtrl.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_pollCtrl_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_powerCfg.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_powerCfg_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_scene.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_scene_attr.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_time.c
    ${SDK_PREFIX}/zigbee/zcl/general/zcl_time_attr.c
    ${SDK_PREFIX}/zigbee/zcl/security_safety/zcl_ias_ace.c
    ${SDK_PREFIX}/zigbee/zcl/security_safety/zcl_ias_wd.c
    ${SDK_PREFIX}/zigbee/zcl/security_safety/zcl_ias_wd_attr.c
    ${SDK_PREFIX}/zigbee/zcl/security_safety/zcl_ias_zone.c
    ${SDK_PREFIX}/zigbee/zcl/security_safety/zcl_ias_zone_attr.c
    ${SDK_PREFIX}/zigbee/zcl/zcl_wwah/zcl_wwah.c
    ${SDK_PREFIX}/zigbee/zcl/zcl_wwah/zcl_wwah_attr.c
    ${SDK_PREFIX}/zigbee/zcl/ota_upgrading/zcl_ota.c
    ${SDK_PREFIX}/zigbee/zcl/ota_upgrading/zcl_ota_attr.c
    ${SDK_PREFIX}/zigbee/common/zb_config.c
    ${SDK_PREFIX}/zigbee/af/zb_af.c
    ${SDK_PREFIX}/zigbee/wwah/wwah.c
    ${SDK_PREFIX}/zigbee/wwah/wwahEpCfg.c
    ${SDK_PREFIX}/zigbee/gp/gp.c
    ${SDK_PREFIX}/zigbee/gp/gpEpCfg.c
    ${SDK_PREFIX}/zigbee/gp/gp_proxyTab.c
    ${SDK_PREFIX}/zigbee/ss/ss_nv.c
    ${SDK_PREFIX}/zigbee/ota/ota.c
    ${SDK_PREFIX}/zigbee/ota/otaEpCfg.c



    ${SDK_PREFIX}/zbhci/zbhci.c
    ${SDK_PREFIX}/zbhci/zbhciCmdProcess.c
    ${SDK_PREFIX}/zbhci/zbhci_zclHandler.c
    ${SDK_PREFIX}/zbhci/uart/hci_uart.c
    ${SDK_PREFIX}/zbhci/usbCDC/zbhci_usb_cdc.c
    ${SDK_PREFIX}/zbhci/usbHID/zbhci_usb_hid.c
    ${SDK_PREFIX}/zbhci/usbPrint/hci_usbPrint.c


    ${SDK_PREFIX}/proj/common/list.c
    ${SDK_PREFIX}/proj/common/mempool.c
    ${SDK_PREFIX}/proj/common/string.c
    ${SDK_PREFIX}/proj/common/tlPrintf.c
    ${SDK_PREFIX}/proj/common/utility.c
    ${SDK_PREFIX}/proj/drivers/drv_adc.c
    ${SDK_PREFIX}/proj/drivers/drv_flash.c
    ${SDK_PREFIX}/proj/drivers/drv_calibration.c
    ${SDK_PREFIX}/proj/drivers/drv_gpio.c
    ${SDK_PREFIX}/proj/drivers/drv_hw.c
    ${SDK_PREFIX}/proj/drivers/drv_i2c.c
    ${SDK_PREFIX}/proj/drivers/drv_keyboard.c
    ${SDK_PREFIX}/proj/drivers/drv_nv.c
    ${SDK_PREFIX}/proj/drivers/drv_pm.c
    ${SDK_PREFIX}/proj/drivers/drv_putchar.c
    ${SDK_PREFIX}/proj/drivers/drv_pwm.c
    ${SDK_PREFIX}/proj/drivers/drv_spi.c
    ${SDK_PREFIX}/proj/drivers/drv_timer.c
    ${SDK_PREFIX}/proj/drivers/drv_uart.c
    ${SDK_PREFIX}/proj/os/ev.c
    ${SDK_PREFIX}/proj/os/ev_buffer.c
    ${SDK_PREFIX}/proj/os/ev_poll.c
    ${SDK_PREFIX}/proj/os/ev_queue.c
    ${SDK_PREFIX}/proj/os/ev_timer.c
)


################################
# Telink Zigbee device

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}

    ${SDK_PREFIX}/zigbee/af
    ${SDK_PREFIX}/zigbee/bdb/includes
    ${SDK_PREFIX}/zigbee/ota
    ${SDK_PREFIX}/zigbee/zcl
    ${SDK_PREFIX}/zigbee/common/includes
    ${SDK_PREFIX}/zigbee/zbapi
    ${SDK_PREFIX}/proj
    ${SDK_PREFIX}/proj/common
    ${SDK_PREFIX}/platform

    # Zigbee sources reference zcl_options.h
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/custom_zcl
)

LINK_DIRECTORIES(
	${SDK_PREFIX}/zigbee/lib/tc32
	${SDK_PREFIX}/platform/lib
)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${SDK_PREFIX}/platform/boot/${TELINK_PLATFORM}/boot_${TELINK_PLATFORM}.link")

file( GLOB SOURCES1 *.c *.cpp *.h *.S common/*.c common/*.h custom_zcl/*.c custom_zcl/*.h )

SET (SOURCES  ${SOURCES1} ${ZIGBEE_SRC})

ADD_EXECUTABLE(${TARGET} ${SOURCES})
TARGET_LINK_LIBRARIES(${TARGET}
        -Wl,--start-group

	#ZigBee

	zb_ed
	drivers_${TELINK_PLATFORM}

        -Wl,--end-group
)

ADD_BIN_TARGET(${TARGET} ${PROJECT_SOURCE_DIR}/tools)
ADD_OTA_TARGET(${TARGET} ${PROJECT_SOURCE_DIR}/tools)
