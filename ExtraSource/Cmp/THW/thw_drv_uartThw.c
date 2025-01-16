/*
 * thw_menuThw.c
 *
 *  Created on: 26 jan 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * la communincation utilisé par le THW\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>

const char thw_drv_uartThw_menuName[] = "Driver | Uart THW";

//-------------------------
#include <THW/thw.h>


st_thw_menuItem thw_drv_uartThw_menuTab[] = {
};
uint16_t thw_drv_uartThw_menuTabSize = sizeof(thw_drv_uartThw_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_uartThw_DisplayMenu(void);
static void thw_drv_uartThw_ManageChoice(char CodeToManage);
static void thw_drv_uartThw_RefreshFn(void);

static uint8_t thw_drv_uartThw_cpt = 0;

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_uartThw_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_uartThw_setActive(void)
{
	thw_drv_uartThw_cpt = 0;

	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_drv_uartThw_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_uartThw_ManageChoice;
	thw_actualMenu.refreshFn = 		thw_drv_uartThw_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 50;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_uartThw_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_uartThw_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_uartThw_menuName);
	THW_printf("\r\n");

	THW_printf("cpt :\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_drv_uartThw_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_drv_uartThw_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_uartThw_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_uartThw_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_uartThw_menuTabSize)){
		if(thw_drv_uartThw_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_uartThw_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_uartThw_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_uartThw_RefreshFn(void)
{
	THW_goto(3,7);
	THW_printf("%d"VT100_CLEAREOL, thw_drv_uartThw_cpt++);

}




//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************

#endif //MODE_THW
