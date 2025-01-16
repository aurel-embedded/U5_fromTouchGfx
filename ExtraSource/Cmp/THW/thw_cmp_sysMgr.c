
/*
 * thw_cmp_sysMgr.c
 *
 *  Created on: 12 oct. 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant SysMgr\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <FDC/fdc_api.h>
#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <SysMgr_Core/SysMgr_api.h>
#include "fdc_config.h"



const char thw_cmp_sysMgr_menuName[] = "Component | SysMgr";

//-----------------------------------------------------------------------------
// CONFIGURATION
//-----------------------------------------------------------------------------

//-------------------------

static void 	thw_cmp_sysMgr_sendError(void);
static void 	thw_cmp_sysMgr_sendWarning(void);
static void 	thw_cmp_sysMgr_sendStateToOK(void);
static void 	thw_cmp_sysMgr_SetStateToDefault(void);


st_thw_menuItem thw_cmp_sysMgr_menuTab[] = {
		{.name = "Send Error for all components", 							.pActionFn = thw_cmp_sysMgr_sendError, 			.info = 0},
		{.name = "Send Warnings for all components", 						.pActionFn = thw_cmp_sysMgr_sendWarning, 		.info = 0},
		{.name = "Set and send Status for all components to OK  ",			.pActionFn = thw_cmp_sysMgr_sendStateToOK, 		.info = 0},
		{.name = "Set and send Status for all components to Default ",		.pActionFn = thw_cmp_sysMgr_SetStateToDefault, 	.info = 0},
};

uint16_t thw_cmp_sysMgr_menuTabSize = sizeof(thw_cmp_sysMgr_menuTab) / sizeof(st_thw_menuItem);


static void thw_cmp_sysMgr_DisplayMenu(void);
static void thw_cmp_sysMgr_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_sysMgr_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_sysMgr_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_sysMgr_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_sysMgr_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;

	// Init FDC
	FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size);

	// Init SYS MGR
	SYSMGR_init();

}

//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_sysMgr_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_sysMgr_DisplayMenu(void)
{
	// Affichage Menu
	THW_Banner(thw_cmp_sysMgr_menuName);
	THW_printf("\r\n");

	THW_printf("  nbOfErrorMsgReceived    : %d  \r\n", sysMgr_internalData.nbOfErrorMsgReceived);
	THW_printf("  nbOfWarningMsgReceived  : %d  \r\n", sysMgr_internalData.nbOfWarningMsgReceived);
	THW_printf("  nbOfStatusMsgReceived   : %d  \r\n", sysMgr_internalData.nbOfStatusMsgReceived);
	THW_printf("\r\n");
	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_sysMgr_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_sysMgr_menuTab[ind].name);
	}
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_sysMgr_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_sysMgr_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_sysMgr_menuTabSize)){
		if(thw_cmp_sysMgr_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_sysMgr_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		SYSMGR_exit();
		FDC_exit(&fdc1_hdl);
		// Return to the previous menu
		thw_cmp_setActive();
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_sysMgr_sendError(void)
/// \brief
//------------------------------------------------------------------------------
static void 	thw_cmp_sysMgr_sendError(void)
{
	for (int i = 0; i < sysMgr_Error_cfgLst_size; i++)
	{
		SYSMGR_setError(sysMgr_Error_cfgLst[i].ID,  i+1);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_sysMgr_sendWarning(void)
/// \brief
//------------------------------------------------------------------------------
static void 	thw_cmp_sysMgr_sendWarning(void)
{
	for (int i = 0; i < sysMgr_Warning_cfgLst_size; i++)
	{
		SYSMGR_setWarning(sysMgr_Warning_cfgLst[i].ID,  i+1);
	}
}

//------------------------------------------------------------------------------
/// \fn 		static void thw_cmp_sysMgr_sendStateToOK(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_sysMgr_sendStateToOK(void)
{
	for (int i = 0; i < sysMgr_Status_cfgLst_size; i++)
	{
		SYSMGR_setStatus(sysMgr_Status_cfgLst[i].ID,  CMP_STATUS_OK);
	}
}


//------------------------------------------------------------------------------
/// \fn 		static void thw_cmp_sysMgr_SetStateToDefault(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_sysMgr_SetStateToDefault(void)
{
	for (int i = 0; i < sysMgr_Status_cfgLst_size; i++)
	{
		SYSMGR_setStatus(sysMgr_Status_cfgLst[i].ID,  CMP_STATUS_DEFAULT);
	}
}


#endif //MODE_THW


