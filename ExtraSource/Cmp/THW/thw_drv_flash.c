/*
 * thw_menuMemory.c
 *
 *  Created on: 21 Mars. 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * la mémoire Flash.\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"
#include "drvFlash/DrvFlash.h"

const char thw_drv_flash_menuName[] = "Driver | FLASH";


static void thw_drv_flash_toggleArea(void);
static void thw_drv_flash_eraseSector(void);
static void thw_drv_flash_write1Data(void);
static void thw_drv_flash_writeAllSector(void);

st_thw_menuItem thw_drv_flash_menuTab[] = {
		{.name = "Toggle Flash Area", 	.pActionFn = thw_drv_flash_toggleArea, 		.info = 0},
		{.name = "", 					.pActionFn = NULL, 							.info = 0},
		{.name = "Erase Sector", 		.pActionFn = thw_drv_flash_eraseSector, 	.info = 0},
		{.name = "Write 1 Data", 		.pActionFn = thw_drv_flash_write1Data, 		.info = 0},
		{.name = "Write All sector", 	.pActionFn = thw_drv_flash_writeAllSector, 	.info = 0},
};
uint16_t thw_drv_flash_menuTabSize = sizeof(thw_drv_flash_menuTab) / sizeof(st_thw_menuItem);

uint8_t thw_drv_flash_data[] = {	0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
									0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
uint8_t thw_drv_flash_data_size = sizeof(thw_drv_flash_data)/sizeof(uint8_t);

//****************************************
//	*** MEMORY prototype encapsulation ***
//****************************************

static void thw_drv_flash_DisplayMenu(void);
static void thw_drv_flash_ManageChoice(char CodeToManage);

//	uint8_t 	bank = FLASH_BANK_1;
//	uint8_t 	sector = 127;
//	uint16_t 	sectorQty = 1;
//	uint32_t	add = 0x080FE000;

typedef struct{
	uint8_t 	bank;
	uint8_t 	sector;
//	uint16_t 	sectorQty;
//	uint32_t	address;
}thw_drv_flash_memArea_t;

static thw_drv_flash_memArea_t thw_drv_flash_memAreaList[] = {
		{.bank = FLASH_BANK_1, .sector = 127},
		{.bank = FLASH_BANK_2, .sector = 0},
		{.bank = FLASH_BANK_2, .sector = 1},
		{.bank = FLASH_BANK_2, .sector = 2},
		{.bank = FLASH_BANK_2, .sector = 15},
		{.bank = FLASH_BANK_2, .sector = 16},
		{.bank = FLASH_BANK_2, .sector = 17},
		{.bank = FLASH_BANK_2, .sector = 30},
};
static uint8_t thw_drv_flash_memAreaList_size = sizeof(thw_drv_flash_memAreaList) / sizeof(thw_drv_flash_memArea_t);
static uint8_t thw_drv_flash_memAreaList_ind = 0;
static void thw_drv_flash_displaySelection(void);
static uint32_t thw_drv_flash_sectorToAddress(thw_drv_flash_memArea_t *pMemArea);

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_displayFrameSelection(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_displaySelection(void)
{
	for(uint8_t i = 0; i < thw_drv_flash_memAreaList_size; i++){
		if(i == thw_drv_flash_memAreaList_ind)
			THW_printf(" -->");
		else
			THW_printf("    ");
		THW_printf(" bank: %d, sector: %d, address: 0x%08X",
				thw_drv_flash_memAreaList[i].bank,
				thw_drv_flash_memAreaList[i].sector,
				thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[i])));
		THW_printf(VT100_CLEAREOL"\r\n");
	}
}


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_flash_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_flash_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_drv_flash_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_flash_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;

	// Init Flash
	if(DrvFlashInit() != FLASH_SUCCESS){
		THW_printf("THW - Test Memoire - ERROR : DrvFlashInit()\r\n");
		thw_drv_setActive();		// back to previous menu
	}

	// RAZ screen
	THW_clearScreen();

}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_drv_flash_menuName);
	THW_printf("\r\n");

	THW_printf("Memory Area Selection:\r\n");
	thw_drv_flash_displaySelection();
	THW_printf("\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_displayActionMenu(thw_drv_flash_menuTab, thw_drv_flash_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_flash_menuTabSize)){
		if(thw_drv_flash_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_flash_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		DrvFlashDeinit();			// Deinit Flash
		thw_drv_setActive();		// Return to the previous menu
	}
}




//==============================================================================
//								FLASH TESTS
//==============================================================================

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_toggleArea(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_toggleArea(void)
{
	thw_drv_flash_memAreaList_ind = (thw_drv_flash_memAreaList_ind + 1) % thw_drv_flash_memAreaList_size;
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_eraseSector(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_eraseSector(void)
{
	uint8_t ret = 0;

	THW_clearEndOfScreen();
	THW_goto(14 + thw_drv_flash_memAreaList_size + thw_drv_flash_menuTabSize, 0);

	while(DrvFlashGetState() != FLASH_READY){
		osDelay(1);
	}

	// Erase Sector
	ret = DrvFlashErase(
			thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].bank,
			thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].sector,
			1);
	if(ret != FLASH_SUCCESS){
		THW_printf("ERROR Flash Erase (bank: %d, sect: %d, sect qty:%d)\r\n",
				thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].bank,
				thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].sector,
				1);
		THW_avoidClearScreen();
		return;
	}

	THW_printf("SUCCESS Flash Erase (bank: %d, sect: %d, sect qty:%d)\r\n",
			thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].bank,
			thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind].sector,
			1);


	THW_avoidClearScreen();
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_writeData(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_write1Data(void)
{
	uint8_t ret = 0;

	THW_clearEndOfScreen();
	THW_goto(14 + thw_drv_flash_memAreaList_size + thw_drv_flash_menuTabSize, 0);

	// Write Data
	ret = DrvFlashDataWrite(
			thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])),
			thw_drv_flash_data,
			thw_drv_flash_data_size);
	if(ret != FLASH_SUCCESS){
		THW_printf("ERROR Flash Data Write (add: 0x%08X, size: %d bytes)\r\n",
				thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])),
				thw_drv_flash_data_size);
		THW_avoidClearScreen();
		return;
	}

	THW_printf("SUCCESS Flash Write (add: 0x%08X, size: %d bytes)\r\n",
			thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])),
			thw_drv_flash_data_size);

	THW_avoidClearScreen();
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_writeData(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_flash_writeAllSector(void)
{
	uint8_t ret = 0;
	uint32_t size = 0;

	THW_clearEndOfScreen();
	THW_goto(14 + thw_drv_flash_memAreaList_size + thw_drv_flash_menuTabSize, 0);

	// Write Data
	for(uint16_t ind = 0; ind < FLASH_SECTOR_SIZE; ind += FLASH_PROGRAM_SIZE * 4){
		ret = DrvFlashDataWrite(
				thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])) + ind,
				thw_drv_flash_data,
				thw_drv_flash_data_size);
		if(ret != FLASH_SUCCESS){
			THW_printf("ERROR Flash Data Write (add: 0x%08X, size: %d bytes)\r\n",
					thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])),
					size);
			THW_avoidClearScreen();
			return;
		}
		size += thw_drv_flash_data_size;
	}

	THW_printf("SUCCESS Flash Write (add: 0x%08X, size: %d bytes)\r\n",
			thw_drv_flash_sectorToAddress(&(thw_drv_flash_memAreaList[thw_drv_flash_memAreaList_ind])),
			size);

	THW_avoidClearScreen();
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_flash_sectorToAddress(void)
/// \brief
//------------------------------------------------------------------------------
static uint32_t thw_drv_flash_sectorToAddress(thw_drv_flash_memArea_t *pMemArea)
{
	if(pMemArea->bank == FLASH_BANK_1){
		return ADDR_FLASH_BANK1(pMemArea->sector);
	}else{
		return ADDR_FLASH_BANK2(pMemArea->sector);
	}
}


#endif //MODE_THW
