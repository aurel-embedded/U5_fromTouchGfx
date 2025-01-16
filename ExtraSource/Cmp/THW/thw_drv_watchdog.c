/*
 * thw_drv_wdg.c
 *
 *  Created on: 06 fev 2024
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * le watchdog\n
 *
 */
#ifdef MODE_THW

#include <string.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <WATCHDOG/watchdog_api.h>



const char thw_drv_wdg_menuName[] = "Driver | WDG";


//-------------------------
static void thw_drv_wdg_startCmp(void);
static void thw_drv_wdg_toggleWatchdogRefresh(void);
static void thw_drv_wdg_testBlockingWait(void);

st_thw_menuItem thw_drv_wdg_menuTab[] = {
		{.name = "Enable Watchdog Component",	.pActionFn = thw_drv_wdg_startCmp, 				.info = 0},
		{.name = "En/Dis Watchdog Refresh",		.pActionFn = thw_drv_wdg_toggleWatchdogRefresh, .info = 0},
		{.name = "Test Blocking Wait",			.pActionFn = thw_drv_wdg_testBlockingWait,	 	.info = 0},
};
uint16_t thw_drv_wdg_menuTabSize = sizeof(thw_drv_wdg_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_wdg_DisplayMenu(void);
static void thw_drv_wdg_ManageChoice(char CodeToManage);
static void thw_drv_wdg_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_wdg_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_wdg_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_wdg_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_wdg_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_wdg_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_wdg_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_wdg_menuName);
	THW_printf("\r\n");

	THW_printf("WDG\r\n");
	THW_printf("  Init LL          : \r\n");
	THW_printf("  Watchdog Refresh : \r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_wdg_menuTab, thw_drv_wdg_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_wdg_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_wdg_menuTabSize)){
		if(thw_drv_wdg_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_wdg_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_wdg_RefreshFn(void)
{
	THW_goto(4, 22); 	(WATCHDOG_initLL_getStatus())? 				THW_printf("Ok \r\n"): 			THW_printf("Ko \r\n");
	THW_goto(5, 22); 	(WATCHDOG_WatchdogRefresh_getStatus())? 	THW_printf("Enable  \r\n"): 	THW_printf("Disable  \r\n");
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_startCmp(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_wdg_startCmp(void)
{
	WATCHDOG_init();
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_toggleWatchdogRefresh(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_wdg_toggleWatchdogRefresh(void)
{
	if(WATCHDOG_WatchdogRefresh_getStatus()){
		WATCHDOG_WatchdogRefresh_disable();
	}else{
		WATCHDOG_WatchdogRefresh_enable();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_wdg_testBlockingWait(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_wdg_testBlockingWait(void)
{
	WATCHDOG_Test();
}



#endif //MODE_THW
