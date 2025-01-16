
/*
 * thw_cmp_Fan.c
 *
 *  Created on: 26 Janv 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Reg\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <app_configFdc.h>
#include <FDC/fdc_api.h>
#include <stdlib.h>
#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <FAN/fan_api.h>


const char thw_cmp_fan_menuName[] = "Component | FAN";

static void thw_cmp_fan_toggleFan(void);

st_thw_menuItem thw_cmp_fan_menuTab[] = {
		{.name = "Toggle Fan", 		.pActionFn = thw_cmp_fan_toggleFan, 	.info = 0},
};
uint16_t thw_cmp_fan_menuTabSize = sizeof(thw_cmp_fan_menuTab) / sizeof(st_thw_menuItem);

static uint8_t thw_cmp_fan_cpt;

static bool 	thw_cmp_fan_fanIsRunning = false;
static uint8_t 	thw_cmp_fan_dutyCycle_min = 0;
static uint8_t 	thw_cmp_fan_dutyCycle_max = 40;
//------------------------------------------------------------------------------
static void thw_cmp_fan_DisplayMenu(void);
static void thw_cmp_fan_ManageChoice(char CodeToManage);
static void thw_cmp_fan_RefreshFn(void);


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fan_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fan_setActive(void)
{
	thw_cmp_fan_cpt = 0;

	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;

	if(FAN_init() != HAL_OK)
		return;
	thw_cmp_fan_fanIsRunning = false;

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_cmp_fan_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_cmp_fan_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_cmp_fan_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fan_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fan_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_fan_menuName);
	THW_printf("\r\n");
	THW_printf("Refresh Running:\r\n");
	THW_printf("\r\n");
	THW_printf("FAN\r\n");
	THW_printf("Fan Component Status :\r\n");
	THW_printf("Fan Status           :\r\n");
	THW_printf("Fan Speed (RPM)      :\r\n");
	THW_printf("Fan Speed (Hz)       :\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_fan_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_fan_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fan_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fan_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_fan_menuTabSize)){
		if(thw_cmp_fan_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_fan_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){

		FAN_exit();
		FDC_exit(&fdc1_hdl);

		// Return to the previous menu
		thw_cmp_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fan_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_cmp_fan_RefreshFn(void)
{
	thw_cmp_fan_cpt++;

	THW_goto(3, 18);
	THW_printf("%d"VT100_CLEAREOL, thw_cmp_fan_cpt);

	THW_goto(6, 24);
	THW_printf("%s"VT100_CLEAREOL, cmpModeToString(FAN_getCmpStatus()));

	THW_goto(7, 24);
	FAN_isFanRunning()? THW_printf("Running"VT100_CLEAREOL): THW_printf("Stopped"VT100_CLEAREOL);

	THW_goto(8, 24);
	THW_printf("%0.0f"VT100_CLEAREOL, FAN_getFanSpeed_inRPM());

	THW_goto(9, 24);
	THW_printf("%0.2f"VT100_CLEAREOL, FAN_getFanSpeed_inHz());
}




//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fan_toggleFan(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fan_toggleFan(void)
{
	thw_cmp_fan_fanIsRunning = !thw_cmp_fan_fanIsRunning;

	if(thw_cmp_fan_fanIsRunning){
		FAN_setFanDutyCycle(thw_cmp_fan_dutyCycle_max);
	}else{
		FAN_setFanDutyCycle(thw_cmp_fan_dutyCycle_min);
	}
}







#endif //MODE_THW
