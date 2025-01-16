/*
 * thw_menuCmpFdc.c
 *
 *  Created on: 10 jan 2023
 *      Author: Larry K.
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Fdcan\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <THW/thw.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>

const char thw_cmp_fdc_menuName[] = "FDC";

//-------------------------

st_thw_menuItem thw_cmp_fdc_menuTab[] = {
		{.name = "Normal Func", 		.pActionFn = thw_cmp_fdc_norm_setActive, 			.info = 0},
		{.name = "Hardware Func", 		.pActionFn = thw_cmp_fdc_Hw_setActive, 				.info = 0},
		{.name = "Bridge Func", 		.pActionFn = thw_cmp_fdc_bridge_setActive, 			.info = 0},
		{.name = "Rx Management", 		.pActionFn = thw_cmp_fdc_rxManagement_setActive, 	.info = 0},
};
uint16_t thw_cmp_fdc_menuTabSize = sizeof(thw_cmp_fdc_menuTab) / sizeof(st_thw_menuItem);


static void thw_cmp_fdc_DisplayMenu(void);
static void thw_cmp_fdc_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fdc_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fdc_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_fdc_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_fdc_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;
}

//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_DisplayMenu(void)
{
	// Affichage Menu
	THW_Banner(thw_cmp_fdc_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_fdc_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_fdc_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_fdc_menuTabSize)){
		if(thw_cmp_fdc_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_fdc_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		// Return to the previous menu
		thw_main_setActive();
	}
}


#endif //MODE_THW

