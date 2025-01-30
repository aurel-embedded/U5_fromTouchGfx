/*
 * thw_main.c
 *
 * Ce composant s'occupe de gérer l'affichage et les actions sur le menu
 *  principal.\n
 * Ces fonctions sont appelées par le composant principal Hardware Test.\n\n
 *
 *  Created on: 7 nov. 2022
 *      Author: apajadon
 */

#include <stddef.h>
#include <stdint.h>
#include <THW/thw.h>
#include <THW_core/THW_testHardware_common.h>

const char thw_main_menuName[] = "Main";

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------
//****************************************
//			*** Main Menu ***
//****************************************
st_thw_menuItem thw_main_menuTab[] = {
		{.name = "Uart THW", 			.pActionFn = thw_drv_uartThw_setActive, .info = 0},
		{.name = "Driver Adc", 			.pActionFn = thw_drv_adc_setActive, 	.info = 0},
		{.name = "Vee", 				.pActionFn = thw_drv_vee_setActive, 	.info = 0},
		{.name = "Vee2", 				.pActionFn = thw_drv_vee2_setActive, 	.info = 0},
//		{.name = "Test HardFault", 		.pActionFn = thw_HardFault_setActive, 	.info = 0},
};


uint16_t thw_main_menuTabSize = sizeof(thw_main_menuTab) / sizeof(st_thw_menuItem);


void thw_MainMenu_DisplayMenu(void);
void thw_MainMenu_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_MainMenu_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_main_setActive(void)
{
	// Menu affiché
	thw_actualMenu.refreshFn = 		NULL;
	thw_actualMenu.displayMenu = 	thw_MainMenu_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_MainMenu_ManageChoice;
	thw_actualMenu.clearScreen = 	true;
}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_MainMenu_DisplayMenu(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_MainMenu_DisplayMenu(void)
{
	THW_printf(VT100_NORMAL);

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_main_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_main_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_main_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("Choix :  ");

}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_MainMenu_ManageChoice(char CodeToManage)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_MainMenu_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_main_menuTabSize)){
		if(thw_main_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_main_menuTab[CodeToManage - 1].pActionFn();
	}
}

