/********************************************************************************************************
 * @file    board_8258_05mmc.h
 *
 * @brief   This is the header file for board_8258_mjwsd05mmc
 *
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

// BUTTON
#define BUTTON1               		GPIO_PA5  // reset test point
#define PA5_FUNC			  		AS_GPIO
#define PA5_OUTPUT_ENABLE	  		0
#define PA5_INPUT_ENABLE	  		1
#define	PULL_WAKEUP_SRC_PA5	  		PM_PIN_PULLUP_10K

#define I2C_CLOCK					100000
#define I2C_GPIO_GROUP				I2C_GPIO_GROUP_C2C3

#define	PM_WAKEUP_LEVEL		  		PM_WAKEUP_LEVEL_LOW

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif


enum{
	VK_SW1 = 0x01,
};

#define	KB_MAP_NORMAL	{\
		{VK_SW1,}, }

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL

#define KB_DRIVE_PINS  {NULL }
#define KB_SCAN_PINS   {BUTTON1}


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
