
/*
 * thw_drv_hotSwap.c
 *
 *  Created on: 15 mai 2024
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * le hot Swap\n
 *
 */
#ifdef MODE_THW

#include <string.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <drvAdc/drvAdc_api.h>
#include <HotSwap/HotSwap_api.h>
#include "fdc_config.h"
#include <FDC/fdc_api.h>

const char thw_drv_hotSwap_menuName[] = "Driver | HotSwap";


//-------------------------
static void thw_drv_hotSwap_toggleAlimBras1(void);
static void thw_drv_hotSwap_toggleRTerm(void);

st_thw_menuItem thw_drv_hotSwap_menuTab[] = {
		{.name = "Alim Bras1       :  Toggle",			.pActionFn = thw_drv_hotSwap_toggleAlimBras1,	.info = 0},
		{.name = "RTerm            :  Toggle",			.pActionFn = thw_drv_hotSwap_toggleRTerm,		.info = 0},
};
uint16_t thw_drv_hotSwap_menuTabSize = sizeof(thw_drv_hotSwap_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_hotSwap_DisplayMenu(void);
static void thw_drv_hotSwap_ManageChoice(char CodeToManage);
static void thw_drv_hotSwap_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_hotSwap_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_hotSwap_setActive(void)
{
	HOTSWAP_Init(false);

	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;


	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_hotSwap_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_hotSwap_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_hotSwap_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_hotSwap_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_hotSwap_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_hotSwap_menuName);
	THW_printf("\r\n");
	THW_printf("  Vref           : \r\n");
	THW_printf("  Vref Value     : \r\n");
	THW_printf("  Alim Bras 1    : \r\n");
	THW_printf("  RTerm          : \r\n");
	THW_printf("\r\n");
	THW_printf("                   Register       Adc            Converted      Physical\r\n");
	THW_printf("  Imon           :\r\n");
	THW_printf("\r\n");
	THW_printf("  Over Current   :\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_hotSwap_menuTab, thw_drv_hotSwap_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_hotSwap_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_hotSwap_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_hotSwap_menuTabSize)){
		if(thw_drv_hotSwap_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_hotSwap_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		HOTSWAP_Exit();
		FDC_exit(&fdc1_hdl);

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_hotSwap_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_hotSwap_RefreshFn(void)
{
	bool 						isUsingVrefInt = DRVADC_isUsingVrefInt();
	bool 						isOverCurrentReached = HOTSWAP_isOverCurrentReached();
	drvAdc_adcVal_t 			adcVal_bras1Imon;
	HOTSWAP_AlimBras1Status 	alimStatus = HOTSWAP_AlimBras1_get();
	HOTSWAP_RTermStatus			rTermStatus = HOTSWAP_RTermStatus_get();
	HOTSWAP_Imon_get(&adcVal_bras1Imon);

	THW_goto(3, 20); 	(isUsingVrefInt)? THW_printf("Internal\r\n"): THW_printf("External\r\n");
	THW_goto(4, 20); 	THW_printf("%d mV"VT100_CLEAREOL, 	DRVADC_getVref());
	THW_goto(5, 20); 	(alimStatus == HOTSWAP_AlimBras1Status_Inactif)?THW_printf("Inactif"VT100_CLEAREOL): THW_printf("Actif"VT100_CLEAREOL);
	THW_goto(6, 20); 	(rTermStatus == HOTSWAP_RTermStatus_Inactif)?THW_printf("Inactif"VT100_CLEAREOL): THW_printf("Actif"VT100_CLEAREOL);
	THW_goto(9, 20); 	THW_printf("%d"VT100_CLEAREOL, 		adcVal_bras1Imon.reg);
	THW_goto(9, 35); 	THW_printf("%d mV"VT100_CLEAREOL, 	adcVal_bras1Imon.Vadc);
	THW_goto(9, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_bras1Imon.Sensor);
	THW_goto(9, 65); 	THW_printf("%.2f mA"VT100_CLEAREOL, adcVal_bras1Imon.Physical);
	THW_goto(11, 20); 	(isOverCurrentReached)? THW_printf("Reached!!!\r\n"): THW_printf("Nominal\r\n");
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_hotSwap_toggleAlimBras1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_hotSwap_toggleAlimBras1(void)
{
	HOTSWAP_AlimBras1Status alimStatus = HOTSWAP_AlimBras1_get();

	// Toggle
	if(alimStatus == HOTSWAP_AlimBras1Status_Inactif){
		alimStatus = HOTSWAP_AlimBras1Status_Actif;
	}else{
		alimStatus = HOTSWAP_AlimBras1Status_Inactif;
	}

	HOTSWAP_AlimBras1_set(alimStatus);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_hotSwap_toggleRTerm(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_hotSwap_toggleRTerm(void)
{
	HOTSWAP_RTermStatus RTermStatus = HOTSWAP_RTermStatus_get();

	// Toggle
	if(RTermStatus == HOTSWAP_RTermStatus_Inactif){
		RTermStatus = HOTSWAP_RTermStatus_Actif;
	}else{
		RTermStatus = HOTSWAP_RTermStatus_Inactif;
	}

	HOTSWAP_RTermStatus_set(RTermStatus);
}


#endif //MODE_THW
