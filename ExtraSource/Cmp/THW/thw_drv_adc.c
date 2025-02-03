#include <UserInterfaces/drvAdc/drvAdc.h>

/*
 * thw_drv_adc.c
 *
 *  Created on: 21 fev 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * les ADC\n
 *
 */

#include <string.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>


const char thw_drv_adc_menuName[] = "Driver | ADC";

static userTypes_6Uint16_t thw_drv_adc_values;

//-------------------------
static void thw_drv_adc_startStop(void);

st_thw_menuItem thw_drv_adc_menuTab[] = {
		{.name = "ADC       :  start/stop",		.pActionFn = thw_drv_adc_startStop, 		.info = 0},
};
uint16_t thw_drv_adc_menuTabSize = sizeof(thw_drv_adc_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_adc_DisplayMenu(void);
static void thw_drv_adc_ManageChoice(char CodeToManage);
static void thw_drv_adc_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_adc_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_adc_setActive(void)
{
	DRVADC_init();

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_adc_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_adc_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_adc_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_adc_menuName);
	THW_printf("\r\n");

	THW_printf("ADC\r\n");
	THW_printf("  State          : \r\n");
	THW_printf("  Potar1 : \r\n");
	THW_printf("  Potar2 : \r\n");
	THW_printf("  Potar3 : \r\n");
	THW_printf("  Potar4 : \r\n");
	THW_printf("  Potar5 : \r\n");
	THW_printf("  Potar6 : \r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_adc_menuTab, thw_drv_adc_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_adc_menuTabSize)){
		if(thw_drv_adc_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_adc_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		DRVADC_stopAdc();
		DRVADC_exit();

		// Return to the previous menu
		thw_main_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_adc_RefreshFn(void)
{
	bool isAdcRunning = DRVADC_isAdcRunning();
	THW_goto(4, 20); 	(isAdcRunning)? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");

	if(isAdcRunning){
		osStatus_t status = DRVADC_getAdcValues(&thw_drv_adc_values);
		if(status == osOK){
			THW_goto(5, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val1);
			THW_goto(6, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val2);
			THW_goto(7, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val3);
			THW_goto(8, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val4);
			THW_goto(9, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val5);
			THW_goto(10, 20); 	THW_printf("%d"VT100_CLEAREOL, thw_drv_adc_values.val6);
		}

	}
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_startStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_startStop(void)
{
	// Toggle
	if(DRVADC_isAdcRunning()){
		DRVADC_stopAdc();
	}else{
		DRVADC_startAdc();
	}
}







