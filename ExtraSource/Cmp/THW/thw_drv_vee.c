
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
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul3.h>
#include <MEM_Core/mem_api.h>
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"
#include "drvFlash/DrvFlash.h"
#include <MEM_Core/VEE/vee_api.h>


const char thw_drv_vee_menuName[] = "Driver | VEE";


//-------------------------
static void 	thw_drv_vee_writeNPacks(uint16_t packQty, vee_number_e vee_number);
static void 	thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty, vee_number_e vee_number);
static void 	thw_drv_vee_veeCleanup(vee_number_e vee_number);
static void 	thw_drv_vee_veeFormat(vee_number_e vee_number);

static void 	thw_drv_vee1_write1Pack(void);
static void 	thw_drv_vee1_write10Pack(void);
static void 	thw_drv_vee1_writeAndVerify10Pack(void);
static void 	thw_drv_vee1_veeCleanup(void);
static void 	thw_drv_vee1_veeFormat(void);

static void 	thw_drv_vee2_write1Pack(void);
static void 	thw_drv_vee2_write10Pack(void);
static void 	thw_drv_vee2_writeAndVerify10Pack(void);
static void 	thw_drv_vee2_veeCleanup(void);
static void 	thw_drv_vee2_veeFormat(void);

static void 	thw_drv_vee3_write1Pack(void);
static void 	thw_drv_vee3_write10Pack(void);
static void 	thw_drv_vee3_writeAndVerify10Pack(void);
static void 	thw_drv_vee3_veeCleanup(void);
static void 	thw_drv_vee3_veeFormat(void);


st_thw_menuItem thw_drv_vee_menuTab[] = {
		{.name = "VEE1 - Write 1 pack", 			.pActionFn = thw_drv_vee1_write1Pack, 			.info = 0},
		{.name = "VEE1 - Write 10 packs", 			.pActionFn = thw_drv_vee1_write10Pack, 			.info = 0},
		{.name = "VEE1 - Write & Verif 10 packs", 	.pActionFn = thw_drv_vee1_writeAndVerify10Pack, .info = 0},
		{.name = "VEE1 - cleanup", 					.pActionFn = thw_drv_vee1_veeCleanup, 			.info = 0},
		{.name = "VEE1 - format", 					.pActionFn = thw_drv_vee1_veeFormat, 			.info = 0},
		{.name = "", 								.pActionFn = NULL, 								.info = 0},
		{.name = "VEE2 - Write 1 pack", 			.pActionFn = thw_drv_vee2_write1Pack, 			.info = 0},
		{.name = "VEE2 - Write 10 packs", 			.pActionFn = thw_drv_vee2_write10Pack, 			.info = 0},
		{.name = "VEE2 - Write & Verif 10 packs", 	.pActionFn = thw_drv_vee2_writeAndVerify10Pack, .info = 0},
		{.name = "VEE2 - cleanup", 					.pActionFn = thw_drv_vee2_veeCleanup, 			.info = 0},
		{.name = "VEE2 - format", 					.pActionFn = thw_drv_vee2_veeFormat, 			.info = 0},
		{.name = "", 								.pActionFn = NULL, 								.info = 0},
		{.name = "VEE3 - Write 1 pack", 			.pActionFn = thw_drv_vee3_write1Pack, 			.info = 0},
		{.name = "VEE3 - Write 10 packs", 			.pActionFn = thw_drv_vee3_write10Pack, 			.info = 0},
		{.name = "VEE3 - Write & Verif 10 packs", 	.pActionFn = thw_drv_vee3_writeAndVerify10Pack, .info = 0},
		{.name = "VEE3 - cleanup", 					.pActionFn = thw_drv_vee3_veeCleanup, 			.info = 0},
		{.name = "VEE3 - format", 					.pActionFn = thw_drv_vee3_veeFormat, 			.info = 0},
};
uint16_t thw_drv_vee_menuTabSize = sizeof(thw_drv_vee_menuTab) / sizeof(st_thw_menuItem);

#define THW_DRV_VEE_OFFSET_DISPLAY 19

//****************************************
//	*** MEMORY prototype encapsulation ***
//****************************************

static void thw_drv_vee_DisplayMenu(void);
static void thw_drv_vee_ManageChoice(char CodeToManage);



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
	if(MEM_init() != mem_error__OK){
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
/// \fn 		void thw_drv_vee_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_drv_vee_menuName);
	THW_printf("\r\n");

	THW_printf("                         VEE1 (Datas/Conf)         VEE2 (Counter)                           VEE3 (Dynamic Data)\r\n");
	THW_printf("Variable Qty (1 pack)  : %d  \r\n", 			VEE_get_variablesQty(MEM_VEE_1));
	THW_goto(4, 56); THW_printf(		"%d  \r\n", 			VEE_get_variablesQty(MEM_VEE_2));
	THW_goto(4, 108); THW_printf(		"%d  \r\n", 			VEE_get_variablesQty(MEM_VEE_3));
	THW_printf("Page Qty               : %d  \r\n", 			VEE_get_pagesQty(MEM_VEE_1));
	THW_goto(5, 56); THW_printf(		"%d  \r\n", 			VEE_get_pagesQty(MEM_VEE_2));
	THW_goto(5, 108); THW_printf(		"%d  \r\n", 			VEE_get_pagesQty(MEM_VEE_3));
	THW_printf("Page Start             : %d\t(0x%08X)  \r\n", 	VEE_get_startPage(MEM_VEE_1), 			VEE_get_startEepromAddress(MEM_VEE_1));
	THW_goto(6, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_startPage(MEM_VEE_2), 		VEE_get_startEepromAddress(MEM_VEE_2));
	THW_goto(6, 108); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_startPage(MEM_VEE_3), 	VEE_get_startEepromAddress(MEM_VEE_3));
	THW_printf("Page End               : %d\t(0x%08X)  \r\n", 	VEE_get_endPage(MEM_VEE_1), 				VEE_get_endEepromAddress(MEM_VEE_1));
	THW_goto(7, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_endPage(MEM_VEE_2), 			VEE_get_endEepromAddress(MEM_VEE_2));
	THW_goto(7, 108); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_endPage(MEM_VEE_3), 		VEE_get_endEepromAddress(MEM_VEE_3));
	THW_printf("Page Active            : %d\t(0x%08X)  \r\n", 	VEE_get_activePage(MEM_VEE_1), 			VEE_get_activePageAddress(MEM_VEE_1));
	THW_goto(8, 56); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_activePage(MEM_VEE_2), 		VEE_get_activePageAddress(MEM_VEE_2));
	THW_goto(8, 108); THW_printf(		"%d\t(0x%08X)  \r\n", 	VEE_get_activePage(MEM_VEE_3), 	VEE_get_activePageAddress(MEM_VEE_3));
	THW_printf("nbMaxElementsByPage    : %d  \r\n", 			VEE_get_nbMaxElementsByPage(MEM_VEE_1));
	THW_goto(9, 56); THW_printf(		"%d  \r\n", 			VEE_get_nbMaxElementsByPage(MEM_VEE_2));
	THW_goto(9, 108); THW_printf(		"%d  \r\n", 			VEE_get_nbMaxElementsByPage(MEM_VEE_3));
	THW_printf("nbMaxWrittenElements   : %d  \r\n", 			VEE_get_nbMaxWrittenElements(MEM_VEE_1));
	THW_goto(10, 56); THW_printf(		"%d  \r\n", 			VEE_get_nbMaxWrittenElements(MEM_VEE_2));
	THW_goto(10, 108); THW_printf(		"%d  \r\n", 			VEE_get_nbMaxWrittenElements(MEM_VEE_3));


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
		MEM_exit();
		thw_drv_setActive();	// Return to the previous menu
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
	thw_drv_vee_writeNPacks(1, MEM_VEE_1);
}



//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_write1Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_write1Pack(void)
{
	thw_drv_vee_writeNPacks(1, MEM_VEE_2);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee3_write1Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee3_write1Pack(void)
{
	thw_drv_vee_writeNPacks(1, MEM_VEE_3);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee1_write10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_write10Pack(void)
{
	thw_drv_vee_writeNPacks(10, MEM_VEE_1);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_write10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_write10Pack(void)
{
	thw_drv_vee_writeNPacks(10, MEM_VEE_2);
}
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee3_write10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee3_write10Pack(void)
{
	thw_drv_vee_writeNPacks(10, MEM_VEE_3);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee1_writeAndVerify10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_writeAndVerify10Pack(void)
{
	thw_drv_vee_writeAndVerifyNPacks(10, MEM_VEE_1);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_writeAndVerify10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_writeAndVerify10Pack(void)
{
	thw_drv_vee_writeAndVerifyNPacks(10, MEM_VEE_2);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee3_writeAndVerify10Pack(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee3_writeAndVerify10Pack(void)
{
	thw_drv_vee_writeAndVerifyNPacks(10, MEM_VEE_3);
}




//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_writeNPacks(uint16_t packQty, bool executeCleanup)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_writeNPacks(uint16_t packQty, vee_number_e vee_number)
{
	uint32_t 	activePage_old = 0;
	vee_error_e writeStatus = vee_error__OK;
	uint32_t tickStart = 0;
	uint32_t time = 0;
	bool 		atLeastOneWriteError_flag = false;
	uint16_t	atLeastOneWriteError_ind = 0;
	vee_error_e	atLeastOneWriteError_statusSav = vee_error__OK;



	switch (vee_number)
	{
	case MEM_VEE_1:
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_1);

		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();

		// Start time watching
		tickStart = HAL_GetTick();

		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------
		if(VEE_get_variablesQty(MEM_VEE_1) != 0){
			for(uint16_t pack = 0; pack < packQty; pack++){
				for(uint16_t i = 0; i < VEE_get_variablesQty(MEM_VEE_1); i++){
					uint16_t virtAdd = i + 1;
					// Write 32 bits (don't write on 0 address)
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_1);

					if(writeStatus != vee_error__OK) {
						// Store first Time atLeastOneWriteError Flag & Ind & Status
						if(atLeastOneWriteError_flag == false){
							atLeastOneWriteError_flag = true;
							atLeastOneWriteError_ind = i;
							atLeastOneWriteError_statusSav = writeStatus;
						}
					}
				}
			}
		}
		break;

	case MEM_VEE_2:
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_2);

		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();

		// Start time watching
		tickStart = HAL_GetTick();

		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------
		if(VEE_get_variablesQty(MEM_VEE_2) != 0){
			for(uint16_t pack = 0; pack < packQty; pack++){
				for(uint16_t i = 0; i < VEE_get_variablesQty(MEM_VEE_2); i++){
					uint16_t virtAdd = i + 1;
					// Write 32 bits (don't write on 0 address)
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_2);

					if(writeStatus != vee_error__OK) {
						// Store first Time atLeastOneWriteError Flag & Ind & Status
						if(atLeastOneWriteError_flag == false){
							atLeastOneWriteError_flag = true;
							atLeastOneWriteError_ind = i;
							atLeastOneWriteError_statusSav = writeStatus;
						}
					}
				}
			}
		}
		break;

	case MEM_VEE_3:
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_3);


		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();

		// Start time watching
		tickStart = HAL_GetTick();

		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------
		if(VEE_get_variablesQty(MEM_VEE_3) != 0){
			for(uint16_t pack = 0; pack < packQty; pack++){
				for(uint16_t i = 0; i < VEE_get_variablesQty(MEM_VEE_3); i++){
					uint16_t virtAdd = i + 1;
					// Write 32 bits (don't write on 0 address)
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_3);

					if(writeStatus != vee_error__OK) {
						// Store first Time atLeastOneWriteError Flag & Ind & Status
						if(atLeastOneWriteError_flag == false){
							atLeastOneWriteError_flag = true;
							atLeastOneWriteError_ind = i;
							atLeastOneWriteError_statusSav = writeStatus;
						}
					}
				}
			}
		}
		break;

	default:
		break;
	}

	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
	THW_clearEndOfScreen();

	// Compute time
	time = (HAL_GetTick() - tickStart);

	// Display writing Error
	if(atLeastOneWriteError_flag == true){
		THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
				atLeastOneWriteError_ind,
				atLeastOneWriteError_statusSav);
	}

	THW_printf("Write done in %d ms      \r\n", time);

	// Display VEE Page (Old --> New)
	if(vee_number == MEM_VEE_1)
	{
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_1));
	}
	else if(vee_number == MEM_VEE_2)
	{
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_2));
	}
	else if(vee_number == MEM_VEE_3 )
	{
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_3));
	}


	THW_avoidClearScreen();

}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty, bool executeCleanup)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_writeAndVerifyNPacks(uint16_t packQty,  vee_number_e vee_number)
{
	uint32_t 	activePage_old = 0;
	vee_error_e 	writeStatus = vee_error__OK;
	vee_error_e 	readStatus = vee_error__OK;
	uint32_t	nbOfVaraibles = 0;
	uint32_t	valRead = 0;

	switch (vee_number) {
	case MEM_VEE_1:
		nbOfVaraibles = VEE_get_variablesQty(MEM_VEE_1);
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_1);
		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();
		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------

		if(VEE_get_variablesQty(MEM_VEE_1)!= 0)
		{
			for(uint16_t pack = 0; pack < packQty; pack++)
			{
				// Write Pack
				for(uint16_t i = 0; i < nbOfVaraibles; i++)
				{
					uint16_t virtAdd = i + 1; // Don't write on 0 address
					// Write 32 bits
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_1);
					if(writeStatus != vee_error__OK)
					{
						THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
								virtAdd, writeStatus);
						return;
					}
				}

			}
			for(uint16_t pack = 0; pack < packQty; pack++)
			{
				// Read Pack & Compare
				for(uint16_t i = 0; i < nbOfVaraibles; i++)
				{
					uint16_t virtAdd = i + 1; // don't read on 0 address
					// Read 32 bits
					readStatus = VEE_read(virtAdd, &valRead, MEM_VEE_1);
					if(readStatus != vee_error__OK) {
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
		}
		THW_printf(" ==> Comparing OK\r\n");
		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_1));
		THW_avoidClearScreen();
		break;

	case MEM_VEE_2:
		nbOfVaraibles = VEE_get_variablesQty(MEM_VEE_2);
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_2);
		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();
		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------

		if(VEE_get_variablesQty(MEM_VEE_2)!= 0)
		{
			for(uint16_t pack = 0; pack < packQty; pack++)
			{

				// Write Pack
				for(uint16_t i = 0; i < nbOfVaraibles; i++)
				{

					uint16_t virtAdd = i + 1; // Don't write on 0 address

					// Write 32 bits
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_2);
					if(writeStatus != vee_error__OK)
					{
						THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
								virtAdd, writeStatus);
						return;
					}
				}
			}
			for(uint16_t pack = 0; pack < packQty; pack++)
			{
				// Read Pack & Compare
				for(uint16_t i = 0; i < nbOfVaraibles; i++)
				{
					uint16_t virtAdd = i + 1; // don't read on 0 address
					// Read 32 bits
					readStatus = VEE_read(virtAdd, &valRead, MEM_VEE_2);
					if(readStatus != vee_error__OK)
					{
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
		}
		THW_printf(" ==> Comparing OK\r\n");
		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_2));
		THW_avoidClearScreen();
		break;

	case MEM_VEE_3:
		nbOfVaraibles = VEE_get_variablesQty(MEM_VEE_3);
		// Get VEE Page before write
		activePage_old = VEE_get_activePage(MEM_VEE_3);
		THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);
		THW_clearEndOfScreen();
		// Write N times Variables (with Cleanup or not)(!!!!!value = item.virtAddrStart + i)
		//------------------------------------------------------------------------------------

		if(VEE_get_variablesQty(MEM_VEE_3)!= 0)
		{
			for(uint16_t pack = 0; pack < packQty; pack++)
			{

				// Write Pack
				for(uint16_t i = 0; i < nbOfVaraibles; i++)
				{

					uint16_t virtAdd = i + 1; // Don't write on 0 address

					// Write 32 bits
					writeStatus = VEE_write(virtAdd, (virtAdd << 16) + i, MEM_VEE_3);
					if(writeStatus != vee_error__OK)
					{
						THW_printf(" ==> At Least one error when writting (&:0x%04X) - Status %d\r\n",
								virtAdd, writeStatus);
						return;
					}
				}
			}
			for(uint16_t pack = 0; pack < packQty; pack++)
			{
				// Read Pack & Compare
				for(uint16_t i = 0; i < nbOfVaraibles ; i++)
				{
					uint16_t virtAdd = i + 1; // don't read on 0 address
					// Read 32 bits
					readStatus = VEE_read(virtAdd, &valRead, MEM_VEE_3);
					if(readStatus != vee_error__OK)
					{
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
		}
		THW_printf(" ==> Comparing OK\r\n");
		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_3));
		THW_avoidClearScreen();
		break;
	default:
		break;
	}



}



//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_veeCleanup(void)
{
	thw_drv_vee_veeCleanup(MEM_VEE_1);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_veeCleanup(void)
{
	thw_drv_vee_veeCleanup(MEM_VEE_2);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee3_veeCleanup(void)
{
	thw_drv_vee_veeCleanup(MEM_VEE_3);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_veeCleanup(vee_number_e vee_number)
{
	uint32_t time,tickStart = 0;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Clean Vee
	switch (vee_number) {
	case MEM_VEE_1:
		VEE_cleanUp(MEM_VEE_1);// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Cleanup in %d ms      \r\n", time);

		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d      \r\n", VEE_get_activePage(MEM_VEE_1));
		THW_avoidClearScreen();
		break;
	case MEM_VEE_2:
		VEE_cleanUp(MEM_VEE_2);// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Cleanup in %d ms      \r\n", time);

		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d      \r\n", VEE_get_activePage(MEM_VEE_2));
		THW_avoidClearScreen();
		break;
	case MEM_VEE_3:
		VEE_cleanUp(MEM_VEE_3);// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Cleanup in %d ms      \r\n", time);

		// Display VEE Page (Old --> New)
		THW_printf("VEE active Page:  %2d      \r\n", VEE_get_activePage(MEM_VEE_3));
		THW_avoidClearScreen();
		break;
	default:
		break;
	}



}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee1_veeFormat(void)
{
	thw_drv_vee_veeFormat(MEM_VEE_1);
}
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_veeFormat(void)
{
	thw_drv_vee_veeFormat(MEM_VEE_2);
}
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee3_veeFormat(void)
{
	thw_drv_vee_veeFormat(MEM_VEE_3);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee_veeFormat(vee_number_e vee_number)
{
	uint32_t activePage_old = 0;
	uint32_t time,tickStart = 0 ;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE_OFFSET_DISPLAY + thw_drv_vee_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Format Vee
	switch (vee_number) {
	case MEM_VEE_1:
		activePage_old = VEE_get_activePage(MEM_VEE_1);
		VEE_format(MEM_VEE_1);
		// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Format in %d ms      \r\n", time);
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_1));
		THW_avoidClearScreen();
		break;
	case MEM_VEE_2:
		activePage_old = VEE_get_activePage(MEM_VEE_2);
		VEE_format(MEM_VEE_2);
		// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Format in %d ms      \r\n", time);
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_2));
		THW_avoidClearScreen();
		break;
	case MEM_VEE_3:
		activePage_old = VEE_get_activePage(MEM_VEE_3);
		VEE_format(MEM_VEE_3);
		// Compute time
		time = (HAL_GetTick() - tickStart);

		THW_printf("Format in %d ms      \r\n", time);
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, VEE_get_activePage(MEM_VEE_3));
		THW_avoidClearScreen();
		break;
	default:
		break;
	}

}

#endif //MODE_THW
