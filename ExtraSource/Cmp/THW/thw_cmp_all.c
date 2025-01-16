/*
 * thw_cmp_all.c
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de démarrer tous les composants\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <cmsis_os2.h>
#include <FDC/fdc_api.h>
#include "fdc_config.h"
#include <THW/thw.h>
#include <THW_core/THW_testHardware_common.h>


const char thw_cmp_all_menuName[] = "Test All Component";

//-------------------------

st_thw_menuItem thw_cmp_all_menuTab[] = {
};
uint16_t thw_cmp_all_menuTabSize = sizeof(thw_cmp_all_menuTab) / sizeof(st_thw_menuItem);

static void thw_cmp_all_DisplayMenu(void);
static void thw_cmp_all_ManageChoice(char CodeToManage);


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_all_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_all_setActive(void)
{
	// Init Components
	FDC_init(&fdc1_hdl, NULL, 0);

	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_all_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_all_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;
}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_all_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_all_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_all_menuName);
	THW_printf("\r\n");
	THW_printf("THW - Test All Component\r\n");

	THW_printf("\r\n");
	THW_printf("ONLY FOR DEBUGGING PURPOSE\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_all_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_all_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_all_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_all_ManageChoice(char CodeToManage)
{
	// Execute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_all_menuTabSize)){
		if(thw_cmp_all_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_all_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		// Finalize Components
		FDC_exit(&fdc1_hdl);

		thw_cmp_setActive();	// Return to the previous menu
	}
}


#endif //MODE_THW


