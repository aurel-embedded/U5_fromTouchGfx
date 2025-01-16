
/*
 * thw_drv_fan.c
 *
 *  Created on: 07 fev 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * le FAN\n
 *
 */
#ifdef MODE_THW

#include <string.h>
#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <FAN/drvFan/drvFan.h>
#include <FAN/drvTachy/drvTachy.h>



const char thw_drv_fan_menuName[] = "Driver | FAN";

#define THW_DRV_FAN_FREQ_DEFAULT 	30
#define THW_DRV_FAN_DC_DEFAULT 		10

static bool 		thw_drv_fan_pwm_isRunning = false;						// Flag for PWM generation
static bool 		thw_drv_fan_capture_isRunning = false;					// Flag for capture
static uint16_t 	thw_drv_fan_pwmFreq_value = THW_DRV_FAN_FREQ_DEFAULT;	// Frequency in kHz
static uint16_t 	thw_drv_fan_pwmDc_value = 	THW_DRV_FAN_DC_DEFAULT;		// Duty Cycle in %

//-------------------------
static void thw_drv_fan_pwmStartStop(void);
static void thw_drv_fan_captureStartStop(void);
static void thw_drv_fan_pwmFreqInc(void);
static void thw_drv_fan_pwmFreqDec(void);
static void thw_drv_fan_pwmDcInc(void);
static void thw_drv_fan_pwmDcDec(void);

st_thw_menuItem thw_drv_fan_menuTab[] = {
		{.name = "PWM      : start/stop",		.pActionFn = thw_drv_fan_pwmStartStop, 		.info = 0},
		{.name = "Capture  : start/stop",		.pActionFn = thw_drv_fan_captureStartStop, 	.info = 0},
		{.name = "PWM      : freq ++", 			.pActionFn = thw_drv_fan_pwmFreqInc, 		.info = 0},
		{.name = "PWM      : freq --", 			.pActionFn = thw_drv_fan_pwmFreqDec, 		.info = 0},
		{.name = "PWM      : Duty Cycle ++", 	.pActionFn = thw_drv_fan_pwmDcInc, 			.info = 0},
		{.name = "PWM      : Duty Cycle --", 	.pActionFn = thw_drv_fan_pwmDcDec, 			.info = 0},
};
uint16_t thw_drv_fan_menuTabSize = sizeof(thw_drv_fan_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_fan_DisplayMenu(void);
static void thw_drv_fan_ManageChoice(char CodeToManage);
static void thw_drv_fan_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_fan_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_fan_setActive(void)
{
	thw_drv_fan_pwm_isRunning = false;
	thw_drv_fan_pwmFreq_value = THW_DRV_FAN_FREQ_DEFAULT;
	thw_drv_fan_pwmDc_value = 	THW_DRV_FAN_DC_DEFAULT;

	DRVTACHY_init();
	DRVFAN_init();
	DRVFAN_setFrequencyInKHz(thw_drv_fan_pwmFreq_value);
	DRVFAN_setDutyCycle(thw_drv_fan_pwmDc_value);

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_fan_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_fan_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_fan_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	200;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_fan_menuName);
	THW_printf("\r\n");

	THW_printf("PWM\r\n");
	THW_printf("  Status      : "); (thw_drv_fan_pwm_isRunning)? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Frequency   : %d kHz\r\n", thw_drv_fan_pwmFreq_value);
	THW_printf("  Duty Cycle  : %d %%\r\n", thw_drv_fan_pwmDc_value);
	THW_printf("\r\n");
	THW_printf("Input Capture\r\n");
	THW_printf("  Status      : "); (thw_drv_fan_capture_isRunning)? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Value       : 0.00 Hz\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_fan_menuTab, thw_drv_fan_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_fan_menuTabSize)){
		if(thw_drv_fan_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_fan_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		thw_drv_fan_pwm_isRunning = false;
		thw_drv_fan_capture_isRunning = false;
		DRVFAN_exit();
		DRVTACHY_exit();

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_fan_RefreshFn(void)
{
	if(thw_drv_fan_capture_isRunning){
		THW_goto(10, 17);
		THW_printf("%0.2f Hz"VT100_CLEAREOL, DRVTACHY_getTachyInHz());
	}
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_pwmStart(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_pwmStartStop(void)
{
	// Toggle
	thw_drv_fan_pwm_isRunning = !thw_drv_fan_pwm_isRunning;

	// Start/Stop PWM
	if(thw_drv_fan_pwm_isRunning){
		DRVFAN_startPwm();
	}else{
		DRVFAN_stopPwm();
	}

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_captureStartStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_captureStartStop(void)
{
	// Toggle
	thw_drv_fan_capture_isRunning = !thw_drv_fan_capture_isRunning;

	// Start/Stop PWM
	if(thw_drv_fan_capture_isRunning){
		DRVTACHY_startTachy();
	}else{
		DRVTACHY_stopTachy();
	}

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_pwmFreqInc(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_pwmFreqInc(void)
{
	if(thw_drv_fan_pwmFreq_value < 40){
		thw_drv_fan_pwmFreq_value++;
		DRVFAN_setFrequencyInKHz(thw_drv_fan_pwmFreq_value);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_pwmFreqDec(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_pwmFreqDec(void)
{
	if(thw_drv_fan_pwmFreq_value > 1){
		thw_drv_fan_pwmFreq_value--;
		DRVFAN_setFrequencyInKHz(thw_drv_fan_pwmFreq_value);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_pwmDcInc(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_pwmDcInc(void)
{
	if(thw_drv_fan_pwmDc_value < 100){
		thw_drv_fan_pwmDc_value = thw_drv_fan_pwmDc_value + 5;
		DRVFAN_setDutyCycle(thw_drv_fan_pwmDc_value);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_fan_pwmDcDec(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_fan_pwmDcDec(void)
{
	if(thw_drv_fan_pwmDc_value > 0){
		thw_drv_fan_pwmDc_value = thw_drv_fan_pwmDc_value - 5;
		DRVFAN_setDutyCycle(thw_drv_fan_pwmDc_value);
	}
}

#endif //MODE_THW
