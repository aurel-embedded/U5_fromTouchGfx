/*
 * thw_menuCmpFdc_hardware.c
 *
 *  Created on: 25 juil. 2023
 *      Author: lkuetche
 */




#ifdef MODE_THW

#include <THW/thw.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>
#include "FDC/fdc_api.h"
#include "fdc_config.h"

const char thw_cmp_fdc_Hw_menuName[] = "Component | FDC | HARDWARE";


/******************Setting Clock Frequency Functions *****************************/
static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_1_Mhz(void);
static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_250_khz(void);
static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_500_khz(void);
static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_125_khz(void);


//-------------------------

st_thw_menuItem thw_cmp_fdc_Hw_menuTab[] = {
		{.name = "Set Fdcan clock 125  khZ", 				.pActionFn = thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_125_khz, .info = 0},
		{.name = "Set Fdcan clock 250  khZ", 				.pActionFn = thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_250_khz, .info = 0},
		{.name = "Set Fdcan clock 500  khZ", 				.pActionFn = thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_500_khz, .info = 0},
		{.name = "Set Fdcan clock 1000 khZ", 				.pActionFn = thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_1_Mhz, 	.info = 0},
};

uint16_t thw_cmp_fdc_Hw_menuTabSize = sizeof(thw_cmp_fdc_Hw_menuTab) / sizeof(st_thw_menuItem);




static void thw_cmp_fdc_Hw_DisplayMenu(void);
static void thw_cmp_fdc_Hw_ManageChoice(char CodeToManage);
static void thw_cmp_fdc_Hw_RefreshFn(void);



//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fdc_Hw_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fdc_Hw_setActive(void)
{

	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_fdc_Hw_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_fdc_Hw_ManageChoice;
	thw_actualMenu.refreshFn = 		thw_cmp_fdc_Hw_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 100;

	FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size);
}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_Hw_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_Hw_DisplayMenu(void)
{
	// Affichage Menu
	THW_Banner(thw_cmp_fdc_Hw_menuName);
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_fdc_Hw_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_fdc_Hw_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_Hw_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_Hw_ManageChoice(char CodeToManage)
{
	// Execute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_fdc_Hw_menuTabSize)){
		if(thw_cmp_fdc_Hw_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_fdc_Hw_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		FDC_exit(&fdc1_hdl);

		thw_cmp_fdc_setActive();;	// Return to the previous menu
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_Hw_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_Hw_RefreshFn(void)
{

	THW_goto(3,10);
}


//==============================================================================
//								ACTIONS
//==============================================================================



/******************Setting Clock Frequency Functions *****************************/
static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_1_Mhz(void)
{
	FDC_Clk_SetBaudRate(&fdc1_hdl, FDC_PRESCALER_1_MHZ);
}

static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_500_khz(void)
{
	FDC_Clk_SetBaudRate(&fdc1_hdl, FDC_PRESCALER_500_KHZ);
}

static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_250_khz(void)
{
	FDC_Clk_SetBaudRate(&fdc1_hdl, FDC_PRESCALER_250_KHZ);
}

static void thw_cmp_fdc_Hw_set_Fdcan_ClkFrequency_125_khz(void)
{
	FDC_Clk_SetBaudRate(&fdc1_hdl, FDC_PRESCALER_125_KHZ);
}




#endif //MODE_THW
