
/*
 * thw_menuCmp_Fdc_rxManagement.c
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
#include <CPT_Core/cpt_api.h>
#include <MEM_Core/mem_api.h>
#include <MONITOR/monitor_api.h>


const char thw_cmp_fdc_rxManagement_menuName[] = "Component | FDC | RxManagement";

static void thw_cmp_fdc_rxManagement_toggleEnable(void);

st_thw_menuItem thw_cmp_fdc_rxManagement_menuTab[] = {
		{.name = "Toggle Rx Management", 	.pActionFn = thw_cmp_fdc_rxManagement_toggleEnable, 	.info = 0},
};
uint16_t thw_cmp_fdc_rxManagement_menuTabSize = sizeof(thw_cmp_fdc_rxManagement_menuTab) / sizeof(st_thw_menuItem);

static void thw_cmp_fdc_rxManagement_DisplayMenu(void);
static void thw_cmp_fdc_rxManagement_ManageChoice(char CodeToManage);
static void thw_cmp_fdc_rxManagement_RefreshFn(void);




//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fdc_rxManagement_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fdc_rxManagement_setActive(void)
{
	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;
	if(FDC_init(&fdc2_hdl, fdc2_filterTab, fdc2_filterTab_size) != HAL_OK)
		return;

//	HAL_FDCAN_ConfigRxFifoOverwrite(fdc1_hdl.hfdcan, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_BLOCKING);
//	HAL_FDCAN_ConfigRxFifoOverwrite(fdc1_hdl.hfdcan, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_BLOCKING);
	HAL_FDCAN_ConfigRxFifoOverwrite(fdc1_hdl.hfdcan, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE);
	HAL_FDCAN_ConfigRxFifoOverwrite(fdc1_hdl.hfdcan, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_OVERWRITE);


	// Init MEM
	MEM_init();

	// Init CPT
	CPT_init();

	// Init Monitor
	MONITOR_init();


	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_fdc_rxManagement_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_fdc_rxManagement_ManageChoice;
	thw_actualMenu.refreshFn = 		thw_cmp_fdc_rxManagement_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 200;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_rxManagement_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_rxManagement_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_fdc_rxManagement_menuName);
	THW_printf("MONITOR, MEM & CPT On\r\n");
	THW_printf("Rx Management Status: %d\r\n", FDC_getRxManagementStatus(&fdc1_hdl));

	THW_printf("\r\n");
	THW_printf("cpt Fifo Hw Overflow:\r\n");

	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_fdc_rxManagement_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_fdc_rxManagement_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_rxManagement_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_rxManagement_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_fdc_rxManagement_menuTabSize)){
		if(thw_cmp_fdc_rxManagement_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_fdc_rxManagement_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){

		MONITOR_exit();
		CPT_exit();
		MEM_exit();

		// Finalize Component
		FDC_exit(&fdc1_hdl);
		FDC_exit(&fdc2_hdl);

		// Return to the previous menu
		thw_cmp_fdc_setActive();
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_rxManagement_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_cmp_fdc_rxManagement_RefreshFn(void)
{
	fdc_Stats_counters_st *pStat1 = FDC_getStat(&fdc1_hdl);

	THW_goto(5,23);  THW_printf("%ld", pStat1->cpt_fifoHwRxOverflow);
}





//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_sendMesg_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_rxManagement_toggleEnable(void)
{
	uint8_t enDis = FDC_getRxManagementStatus(&fdc1_hdl);
	FDC_setRxManagementStatus(&fdc1_hdl, !enDis);
}


#endif //MODE_THW
