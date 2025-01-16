/*
 * hwt_menuMemory.c
 *
 *  Created on: 21 Mars. 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant HWT s'occupe de gérer les test hard concernant
 * la mémoire Flash.\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul_types.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul2.h>
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"
#include "drvFlash/DrvFlash.h"


const char thw_drv_veeF_menuName[] = "Driver | VEE Format";

typedef struct{
	uint32_t 	variableCpt;

	EE_Status 	(*cleanUp)(void);
	EE_Status 	(*format)(EE_Erase_type EraseType);
	EE_Status 	(*writeVariable32bits)(uint16_t VirtAddress, uint32_t Data);
	uint32_t 	(*get_nbOfVariables)(void);
	uint32_t 	(*get_activePage)(void);
}thw_drv_veeF_confItem_t;

thw_drv_veeF_confItem_t thw_drv_veeF_confItem_vee1 = {
		.variableCpt = 			0,
		.writeVariable32bits = 	EE_WriteVariable32bits,
		.cleanUp = 				EE_CleanUp,
		.format = 				EE_Format,
		.get_nbOfVariables = 	EE_ex_get_variablesQty,
		.get_activePage = 		EE_ex_get_activePage,
};

thw_drv_veeF_confItem_t thw_drv_veeF_confItem_vee2 = {
		.variableCpt = 			0,
		.writeVariable32bits = 	EE2_WriteVariable32bits,
		.cleanUp = 				EE2_CleanUp,
		.format = 				EE2_Format,
		.get_nbOfVariables = 	EE2_ex_get_variablesQty,
		.get_activePage = 		EE2_ex_get_activePage,
};

//-------------------------
static void 	thw_drv_veeF_veeFormat(thw_drv_veeF_confItem_t * pVeeCfg);
static void 	thw_drv_vee1_veeFormat(void);
static void 	thw_drv_vee2_veeFormat(void);


st_thw_menuItem thw_drv_veeF_menuTab[] = {
		{.name = "VEE1 - format", 				.pActionFn = thw_drv_vee1_veeFormat, 	.info = 0},
		{.name = "VEE2 - format", 				.pActionFn = thw_drv_vee2_veeFormat, 	.info = 0},
};
uint16_t thw_drv_veeF_menuTabSize = sizeof(thw_drv_veeF_menuTab) / sizeof(st_thw_menuItem);

#define THW_DRV_VEE_OFFSET_DISPLAY 19

//****************************************
//	*** MEMORY prototype encapsulation ***
//****************************************

static void thw_drv_veeF_DisplayMenu(void);
static void thw_drv_veeF_ManageChoice(char CodeToManage);



//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_veeF_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_veeF_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_drv_veeF_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_veeF_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;

	// Init Flash
	if(DrvFlashInit() != FLASH_SUCCESS){
		THW_printf("HWT - Test Memoire - ERROR : DrvFlashInit()\r\n");
		thw_drv_setActive();		// back to previous menu
	}
}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_veeF_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_veeF_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_drv_veeF_menuName);
	THW_printf("\r\n");

	THW_printf("                         VEE1 (Counter)                VEE2 (Stats)\r\n");
	THW_printf("Variable Qty (1 pack)  : %d  \r\n", 			EE_ex_get_variablesQty());
	THW_goto(4, 56); THW_printf(		"%d  \r\n", 			EE2_ex_get_variablesQty());
	THW_printf("Page Qty               : %d  \r\n", 			EE_ex_get_pagesQty());
	THW_goto(5, 56); THW_printf(		"%d  \r\n", 			EE2_ex_get_pagesQty());
	THW_printf("Page Start             : %d\t(0x%08X)  \r\n", 	EE_ex_get_startPage(), 		EE_ex_get_startEepromAddress());
	THW_goto(6, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	EE2_ex_get_startPage(), 	EE2_ex_get_startEepromAddress());
	THW_printf("Page End               : %d\t(0x%08X)  \r\n", 	EE_ex_get_endPage(), 		EE_ex_get_endEepromAddress());
	THW_goto(7, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	EE2_ex_get_endPage(), 		EE2_ex_get_endEepromAddress());
	THW_printf("Page Active            : %d\t(0x%08X)  \r\n", 	EE_ex_get_activePage(), 	EE_ex_get_activePageAddress());
	THW_goto(8, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	EE2_ex_get_activePage(), 	EE2_ex_get_activePageAddress());
	THW_printf("nbMaxElementsByPage    : %d  \r\n", 			EE_ex_get_nbMaxElementsByPage());
	THW_goto(9, 56); THW_printf(		"%d  \r\n", 			EE2_ex_get_nbMaxElementsByPage());
	THW_printf("nbMaxWrittenElements   : %d  \r\n", 			EE_ex_get_nbMaxWrittenElements());
	THW_goto(10, 56); THW_printf(		"%d  \r\n", 			EE2_ex_get_nbMaxWrittenElements());


	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_displayActionMenu(thw_drv_veeF_menuTab, thw_drv_veeF_menuTabSize);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_veeF_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_veeF_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_veeF_menuTabSize)){
		if(thw_drv_veeF_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_veeF_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		DrvFlashDeinit();			// Deinit Flash
		thw_drv_setActive();	// Return to the previous menu
	}
}




//==============================================================================
//									VEE TESTS
//==============================================================================

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_veeF_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_veeFormat(void)
{
	thw_drv_veeF_veeFormat(&thw_drv_veeF_confItem_vee1);
}
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_veeF_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_veeFormat(void)
{
	thw_drv_veeF_veeFormat(&thw_drv_veeF_confItem_vee2);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_veeF_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_veeF_veeFormat(thw_drv_veeF_confItem_t * pVeeCfg)
{
	// Get VEE Pages
	if(pVeeCfg->get_activePage == NULL){
		return;
	}
	uint32_t activePage_old = 	pVeeCfg->get_activePage();

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_veeF_menuTabSize, 0);

	// Start time watching
	uint32_t tickStart = HAL_GetTick();

	// Clean Vee
	if(pVeeCfg->format == NULL){
		return;
	}
	pVeeCfg->format(EE_FORCED_ERASE);

	// Compute time
	uint32_t time = (HAL_GetTick() - tickStart);

	THW_printf("Format in %d ms      \r\n", time);

	// Display VEE Page (Old --> New)
	THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, pVeeCfg->get_activePage());


	THW_avoidClearScreen();

}

#endif //MODE_THW

