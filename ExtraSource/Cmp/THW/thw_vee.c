#include <MEM_Core/VEE/vee_api.h>

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
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"


const char thw_drv_vee_menuName[] = "Driver | VEE";


//-------------------------
static void 	thw_drv_vee_writeNPacks(uint16_t packQty);
static void 	thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty);
static void 	thw_drv_vee_veeCleanup();
static void 	thw_drv_vee_veeFormat();

static void 	thw_drv_vee1_write1Pack(void);
static void 	thw_drv_vee1_write10Pack(void);
static void 	thw_drv_vee1_writeAndVerify10Pack(void);


st_thw_menuItem thw_drv_vee_menuTab[] = {
		{.name = "VEE1 - Write 1 pack", 			.pActionFn = thw_drv_vee1_write1Pack, 			.info = 0},
		{.name = "VEE1 - Write 10 packs", 			.pActionFn = thw_drv_vee1_write10Pack, 			.info = 0},
		{.name = "VEE1 - Write & Verif 10 packs", 	.pActionFn = thw_drv_vee1_writeAndVerify10Pack, .info = 0},
		{.name = "VEE1 - cleanup", 					.pActionFn = thw_drv_vee_veeCleanup, 			.info = 0},
		{.name = "VEE1 - format", 					.pActionFn = thw_drv_vee_veeFormat, 			.info = 0},
};
uint16_t thw_drv_vee_menuTabSize = sizeof(thw_drv_vee_menuTab) / sizeof(st_thw_menuItem);

#define THW_DRV_VEE_OFFSET_DISPLAY 19

//****************************************
//	*** MEMORY prototype encapsulation ***
//****************************************

static void thw_drv_vee_DisplayMenu(void);
static void thw_drv_vee_ManageChoice(char CodeToManage);

uint16_t thw_drv_vee_varQty = 10;


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_vee_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_vee_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_drv_vee_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_vee_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;

	// Init MEM
	if(VEE_init() != vee_error__OK){
		THW_printf("HWT - Test Memoire - ERROR : VEE_init()\r\n");
		thw_main_setActive();		// back to previous menu
	}

	thw_drv_vee_varQty = EE_ex_get_variablesQty();
}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_drv_vee_menuName);
	THW_printf("\r\n");

	THW_printf("                         VEE\r\n");
	THW_printf("Variable Qty (1 pack)  : %d\t(max: %d)\r\n", 		thw_drv_vee_varQty, EE_ex_get_variablesQty());
	THW_printf("Page Active            : %d\t(0x%08X)  \r\n", 		EE_ex_get_activePage(), 		EE_ex_get_activePageAddress());
	THW_printf("pagesQty               : %d  (from %d to %d)\r\n", 	EE_ex_get_pagesQty(), EE_ex_get_startPage(), EE_ex_get_endPage());
	THW_printf("nbMaxElementsByPage    : %d  \r\n", 				EE_ex_get_nbMaxElementsByPage());
	THW_printf("nbMaxWrittenElements   : %d  \r\n", 				EE_ex_get_nbMaxWrittenElements());


	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_displayActionMenu(thw_drv_vee_menuTab, thw_drv_vee_menuTabSize);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_vee_menuTabSize)){
		if(thw_drv_vee_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_vee_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		EE_CleanUp();			// Cleanup
		thw_main_setActive();	// Return to the previous menu
	}
}




//==============================================================================
//									VEE TESTS
//==============================================================================

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee1_write1Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_write1Pack(void)
{
	thw_drv_vee_writeNPacks(1);
}



//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee1_write10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_write10Pack(void)
{
	thw_drv_vee_writeNPacks(10);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee1_writeAndVerify10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_writeAndVerify10Pack(void)
{
	thw_drv_vee_writeAndVerifyNPacks(10);
}




//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_writeNPacks(uint16_t packQty, bool executeCleanup)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_writeNPacks(uint16_t packQty)
{
	uint32_t 	activePage_old = 0;
	EE_Status writeStatus = EE_OK;
	uint32_t tickStart = 0;
	uint32_t time = 0;
	bool 		atLeastOneWriteError_flag = false;
	uint16_t	atLeastOneWriteError_ind = 0;
	EE_Status	atLeastOneWriteError_statusSav = EE_OK;



	// Get VEE Page before write
	activePage_old = EE_ex_get_activePage();

	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
	THW_clearEndOfScreen();

	// Start time watching
	tickStart = HAL_GetTick();

	// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
	//------------------------------------------------------------------------------------
	if((thw_drv_vee_varQty <= EE_ex_get_variablesQty()) && (thw_drv_vee_varQty != 0)){
		for(uint16_t pack = 0; pack < packQty; pack++){
			for(uint16_t i = 0; i < thw_drv_vee_varQty; i++){
				uint16_t virtAdd = i + 1;
				// Write 32 bits (don't write on 0 address)
				writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i);

				if(writeStatus != EE_OK) {
					// Store first Time atLeastOneWriteError Flag & Ind & Status
					if(atLeastOneWriteError_flag == false){
						atLeastOneWriteError_flag = true;
						atLeastOneWriteError_ind = i;
						atLeastOneWriteError_statusSav = writeStatus;
					}
				}
			}
			// Log page consumption after each pack
			THW_printf("Pack %d written - Active Page: %d\r\n", pack + 1, EE_ex_get_activePage());
		}
	}

	// Compute time
	time = (HAL_GetTick() - tickStart);

	// Display writing Error
	if(atLeastOneWriteError_flag == true){
		THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
				atLeastOneWriteError_ind,
				atLeastOneWriteError_statusSav);
	}

	THW_printf("Write done in %d ms      \r\n", time);

	THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, EE_ex_get_activePage());


	// Additional page state logging for debugging
	THW_printf("Page State Check:\r\n");
	for(uint16_t i = EE_ex_get_startPage(); i <= EE_ex_get_endPage(); i++) {
		THW_printf("Page %d: State = %d\r\n", i, EE_ex_get_pageState(i));
	}

	THW_avoidClearScreen();

}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty, bool executeCleanup)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty)
{
	uint32_t 	activePage_old = 0;
	EE_Status 	writeStatus = EE_OK;
	EE_Status 	readStatus = EE_OK;
	uint32_t	valRead = 0;
	uint32_t tickStart = 0;
	uint32_t time = 0;

	// Get VEE Page before write
	activePage_old = EE_ex_get_activePage();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
	THW_clearEndOfScreen();

	// Start time watching
	tickStart = HAL_GetTick();

	// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
	//------------------------------------------------------------------------------------

	if((thw_drv_vee_varQty <= EE_ex_get_variablesQty()) && (thw_drv_vee_varQty != 0))
	{
		for(uint16_t pack = 0; pack < packQty; pack++)
		{
			// Write Pack
			for(uint16_t i = 0; i < thw_drv_vee_varQty; i++)
			{
				uint16_t virtAdd = i + 1; // Don't write on 0 address
				// Write 32 bits
				writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i);
				if(writeStatus != EE_OK)
				{
					THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
							virtAdd, writeStatus);
					return;
				}
			}
			// Log page consumption after each pack
			THW_printf("Pack %d written - Active Page: %d\r\n", pack + 1, EE_ex_get_activePage());
		}

			// Read Variables & Compare
		for(uint16_t i = 0; i < thw_drv_vee_varQty; i++)
		{
			uint16_t virtAdd = i + 1; // don't read on 0 address
			// Read 32 bits
			readStatus = VEE_read(virtAdd, &valRead);
			if(readStatus != EE_OK) {
				THW_printf(" ==> At Least one error when reading (&:0x%04X) - Status %d\r\n",
						virtAdd, readStatus);
				return;
			}else
			{
				// Compare
				if(valRead != (virtAdd << 16) + i)
				{
					THW_printf(" ==> Comparing: ERROR\r\n");
				}
			}
		}
	}

	// Compute time
	time = (HAL_GetTick() - tickStart);

	THW_printf(" ==> Comparing OK (in %d ms)      \r\n", time);
	// Display VEE Page (Old --> New)
	THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, EE_ex_get_activePage());
	THW_avoidClearScreen();

}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_veeCleanup(void)
{
	uint32_t time,tickStart = 0;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Clean Vee
	EE_CleanUp();// Compute time
	time = (HAL_GetTick() - tickStart);

	THW_printf("Cleanup in %d ms      \r\n", time);

	// Display VEE Page (Old --> New)
	THW_printf("VEE active Page:  %2d      \r\n", EE_ex_get_activePage());
	THW_avoidClearScreen();
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_veeFormat()
{
	uint32_t activePage_old = 0;
	uint32_t time,tickStart = 0 ;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Format Vee
	activePage_old = EE_ex_get_activePage();
	EE_Format(EE_FORCED_ERASE);
	// Compute time
	time = (HAL_GetTick() - tickStart);

	THW_printf("Format in %d ms      \r\n", time);
	THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, EE_ex_get_activePage());
	THW_avoidClearScreen();

}

#endif //MODE_THW
