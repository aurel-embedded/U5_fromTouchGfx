/*
 * thw_drv.c
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * les drivers\n
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

const char thw_drv_menuName[] = "Driver";

//------------------------------------------------------------------------------
// Local constants
//------------------------------------------------------------------------------
//****************************************
//			*** Main Menu ***
//****************************************
st_thw_menuItem thw_drv_menuTab[] = {
		{.name = "Uart THW", 			.pActionFn = thw_drv_uartThw_setActive, 	.info = 0},
//		{.name = "VEE", 				.pActionFn = thw_drv_vee_setActive, 		.info = 0},
//		{.name = "VEE Force Format",	.pActionFn = thw_drv_veeF_setActive, 		.info = 0},
};


uint16_t thw_drv_menuTabSize = sizeof(thw_drv_menuTab) / sizeof(st_thw_menuItem);


void thw_drv_DisplayMenu(void);
void thw_drv_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_setActive(void)
{
	// Menu affiché
	thw_actualMenu.refreshFn = 		NULL;
	thw_actualMenu.displayMenu = 	thw_drv_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_ManageChoice;
	thw_actualMenu.clearScreen = 	true;
}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_DisplayMenu(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_drv_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_drv_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");

}

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_ManageChoice(char CodeToManage)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_menuTabSize)){
		if(thw_drv_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		// Return to the previous menu
		thw_main_setActive();
	}
}

#endif	// MODE_THW
