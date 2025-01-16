/*
 * thw_menuCmp_Fdc_Bridge.c
 *
 *  Created on: 26 jan 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Fdcan\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include "APP.h"
#include "FDC/fdc_api.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include "fdc_config.h"
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>


const char thw_cmp_fdc_bridge_menuName[] = "Component | FDC | Bridge";


static void thw_cmp_fdc_bridge_DisplayMenu(void);
static void thw_cmp_fdc_bridge_ManageChoice(char CodeToManage);




//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fdc_bridge_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fdc_bridge_setActive(void)
{
	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;
	if(FDC_init(&fdc2_hdl, fdc2_filterTab, fdc2_filterTab_size) != HAL_OK)
		return;

	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_fdc_bridge_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_fdc_bridge_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_bridge_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_bridge_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_fdc_bridge_menuName);
	THW_printf("\r\n");

	THW_goto(3,28); THW_printf("BRIDGE MODE Actif");
	THW_printf("\r\n");

	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_bridge_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_bridge_ManageChoice(char CodeToManage)
{
	if(CodeToManage == 0){
		// Finalize Component
		FDC_exit(&fdc1_hdl);
		FDC_exit(&fdc2_hdl);

		// Return to the previous menu
		thw_cmp_fdc_setActive();
	}
}





//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


#endif //MODE_THW
