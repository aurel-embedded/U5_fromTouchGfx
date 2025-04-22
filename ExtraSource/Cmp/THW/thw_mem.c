/*
 * hwt_menuMemory.c
 *
 *  Created on: 10 fev. 2025
 *      Author: apajadon
 *
 * Ce sous-composant du composant HWT s'occupe de gérer les test hard concernant
 * la mémoire Flash.\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */

#include <cmsis_os2.h>
#include <MEM/mem_config.h>
#include <MEM_Core/mem_api.h>
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"


const char thw_mem_menuName[] = "Driver | MEM";


//-------------------------
static void 	thw_mem_read(void);
static void 	thw_mem_write(void);
static void 	thw_mem_toggleId(void);


st_thw_menuItem thw_mem_menuTab[] = {
		{.name = "", 			.pActionFn = thw_mem_read, 			.info = 0},
		{.name = "", 			.pActionFn = thw_mem_write, 		.info = 0},
		{.name = "", 			.pActionFn = thw_mem_toggleId, 		.info = 0},
};
uint16_t thw_mem_menuTabSize = sizeof(thw_mem_menuTab) / sizeof(st_thw_menuItem);

#define THW_MEM_OFFSET_DISPLAY 16

//****************************************
//	*** MEMORY ID ***
//****************************************
typedef struct{
	uint16_t	id;
	uint64_t 	data;
}thw_mem_veeItem_t;

static thw_mem_veeItem_t thw_mem_veeItemList[] = {
};
static uint8_t thw_mem_veeItemList_size = sizeof(thw_mem_veeItemList) / sizeof(thw_mem_veeItem_t);
static uint8_t thw_mem_veeItemList_ind = 0;
static void thw_mem_displayVeeItemSelection(void);

static void thw_mem_DisplayMenu(void);
static void thw_mem_ManageChoice(char CodeToManage);

uint16_t thw_mem_varQty = 10;


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_mem_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_mem_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_mem_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_mem_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;


	// Init MEM
	if(MEM_init() != mem_error__OK){
		THW_printf("HWT - Test Memoire - ERROR : MEM_init()\r\n");
		thw_main_setActive();		// back to previous menu
	}
}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_mem_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_mem_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_mem_menuName);
	THW_printf("\r\n");
	THW_printf("Mem Id:\r\n");
	thw_mem_displayVeeItemSelection();


	// Affichage du menu
	THW_displayActionMenu(thw_mem_menuTab, thw_mem_menuTabSize);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_mem_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_mem_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_mem_menuTabSize)){
		if(thw_mem_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_mem_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		MEM_exit();
		thw_main_setActive();	// Return to the previous menu
	}
}




//==============================================================================
//									VEE TESTS
//==============================================================================

//------------------------------------------------------------------------------
/// \fn 		void thw_mem_read(void)
/// \brief
//------------------------------------------------------------------------------
static void 	thw_mem_read(void)
{

}


//------------------------------------------------------------------------------
/// \fn 		void thw_mem_write(void)
/// \brief
//------------------------------------------------------------------------------
static void 	thw_mem_write(void)
{

}


//------------------------------------------------------------------------------
/// \fn 		void thw_mem_toggleId(void)
/// \brief
//------------------------------------------------------------------------------
static void 	thw_mem_toggleId(void)
{

}

//------------------------------------------------------------------------------
/// \fn 		void thw_mem_displayVeeItemSelection(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_mem_displayVeeItemSelection(void)
{
	for(uint8_t i = 0; i < thw_mem_veeItemList_size; i++){
		if(i == thw_mem_veeItemList_ind)
			THW_printf(" -->");
		else
			THW_printf("    ");
		THW_printf(" %d", thw_mem_veeItemList[i].id);
		THW_printf("%16X ", thw_mem_veeItemList[i].data);
		THW_printf(VT100_CLEAREOL"\r\n");
	}
}

