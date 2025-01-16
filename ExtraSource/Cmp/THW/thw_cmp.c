/*
 * thw_cmp.c
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * les composants\n
 * Ces fonctions sont appelées par le composant principal Hardware Test.\n\n
 *
 *  Created on: 7 nov. 2022
 *      Author: apajadon
 */
#ifdef MODE_THW

#include <stddef.h>
#include <stdint.h>
#include <THW/thw.h>
#include <THW_core/THW_testHardware_common.h>
#include "APP.h"

const char thw_cmp_menuName[] = "Component";

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------
//****************************************
//			*** Main Menu ***
//****************************************
st_thw_menuItem thw_cmp_menuTab[] = {
		{.name = "FDC -    Fdcan", 			.pActionFn = thw_cmp_fdc_setActive, 	.info = 0},
		{.name = "REG -    Regulation",		.pActionFn = thw_cmp_reg_setActive, 	.info = 0},
		{.name = "SysMgr - System Manager",	.pActionFn = thw_cmp_sysMgr_setActive, 	.info = 0},
		{.name = "EvtMgr - Event Manager",	.pActionFn = thw_cmp_evt_setActive, 	.info = 0},
		{.name = "Fan -    Fan Component",	.pActionFn = thw_cmp_fan_setActive, 	.info = 0},
		{.name = "All", 					.pActionFn = thw_cmp_all_setActive, 	.info = 0},
};


uint16_t thw_cmp_menuTabSize = sizeof(thw_cmp_menuTab) / sizeof(st_thw_menuItem);


void thw_cmp_DisplayMenu(void);
void thw_cmp_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_setActive(void)
{
	// Menu affiché
	thw_actualMenu.refreshFn = 		NULL;
	thw_actualMenu.displayMenu = 	thw_cmp_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_ManageChoice;
	thw_actualMenu.clearScreen = 	true;
}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_DisplayMenu(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");

}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_ManageChoice(char CodeToManage)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_menuTabSize)){
		if(thw_cmp_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		// Return to the previous menu
		thw_main_setActive();
	}
}

#endif	// MODE_THW
