
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

#include <cmsis_os2.h>
#include <eeprom_emul.h>
#include <stddef.h>
#include <THW/thw.h>
#include "THW_core/THW_testHardware_common.h"

#define ICACHE_ENABLED                 /* ICache is enabled by default for this example */

const char thw_drv_vee2_menuName[] = "Driver | VEE2";

static void MPU_Config(void);
static void PVD_Config(void);
__IO uint32_t ErasingOnGoing = 0;
uint64_t a_VarData96Tab[NB_OF_VARIABLES*2] = {0};
uint32_t VarValue96bits = 0;
uint32_t VarValue = 0;
uint16_t VirtAddVarTab[NB_OF_VARIABLES*2+1];

//-------------------------
static void 	thw_drv_vee2_writeAndVerify(void);
static void 	thw_drv_vee2_veeCleanup();
static void 	thw_drv_vee2_veeFormat();



st_thw_menuItem thw_drv_vee2_menuTab[] = {
		{.name = "VEE - Write & verify", 	.pActionFn = thw_drv_vee2_writeAndVerify, 	.info = 0},
		{.name = "VEE - cleanup", 			.pActionFn = thw_drv_vee2_veeCleanup, 		.info = 0},
		{.name = "VEE - format", 			.pActionFn = thw_drv_vee2_veeFormat, 		.info = 0},
};
uint16_t thw_drv_vee2_menuTabSize = sizeof(thw_drv_vee2_menuTab) / sizeof(st_thw_menuItem);

#define THW_DRV_VEE2_OFFSET_DISPLAY 19

//****************************************
//	*** MEMORY prototype encapsulation ***
//****************************************

static void thw_drv_vee2_DisplayMenu(void);
static void thw_drv_vee2_ManageChoice(char CodeToManage);


EE_Status thw_drv_vee2_ee_status = EE_OK;

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_vee2_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_vee2_setActive(void)
{
	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_drv_vee2_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_drv_vee2_ManageChoice;
	thw_actualMenu.refreshFn = 		NULL;

	#ifdef ICACHE_ENABLED
	  /* MPU configuration: If ICache is enabled (it is for this example), we need to declare the EEPROM emulated flash area
	  * as non-cacheable to keep the flash readings coherent even after content modification.
	  * Otherwise, when reading a flash line, if it has been cached before and modified since then, the value read
	  * is possibly the cached one and not the new one if the line address has remained in the cache. */
	  MPU_Config();

	  /* Enable ICACHE after testing SR BUSYF and BSYENDF */
	  while((ICACHE->SR & 0x1) != 0x0) {;}
	  while((ICACHE->SR & 0x2) == 0x0) {;}
	  ICACHE->CR |= 0x1;
	#endif

  /* Unlock the Flash Program Erase controller */
	HAL_FLASH_Unlock();


	  /* Set EEPROM emulation firmware to erase all potentially incompletely erased
	     pages if the system came from an asynchronous reset. Conditional erase is
	     safe to use if all Flash operations where completed before the system reset */
	  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SBF) == RESET)
	  {
	    /* System reset comes from a power-on reset: Forced Erase */
	    /* Initialize EEPROM emulation driver (mandatory) */
		  thw_drv_vee2_ee_status = EE_Init(EE_FORCED_ERASE);
	    if(thw_drv_vee2_ee_status != EE_OK)
	    {
	    	THW_printf("EE_Init(EE_FORCED_ERASE) - Error\r\n");
	    }
	  }
	  else
	  {
	    /* Clear the Standby flag */
	    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);

	    /* Check and Clear the Wakeup flag */
	    if (__HAL_PWR_GET_FLAG(PWR_WAKEUP_FLAG2) != RESET)
	    {
	      __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_FLAG2);
	    }

	    /* System reset comes from a STANDBY wakeup: Conditional Erase*/
	    /* Initialize EEPROM emulation driver (mandatory) */
	    thw_drv_vee2_ee_status = EE_Init(EE_CONDITIONAL_ERASE);
	    if(thw_drv_vee2_ee_status != EE_OK) {
	    	THW_printf("EE_Init(EE_CONDITIONAL_ERASE) - Error\r\n");
	    }
	  }

	  PVD_Config();


}


//*****************************************************************************
// 							LOCAL FUNCTIONS
//*****************************************************************************
//********************************
//	*** TestMemory Main Menu ***
//********************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_DisplayMenu(void)
{
	// Affichage Banner
	THW_Banner(thw_drv_vee2_menuName);
	THW_printf("\r\n");

	THW_printf("                         VEE\r\n");
//	THW_printf("Variable Qty (1 pack)  : %d\t(max: %d)\r\n", 		thw_drv_vee2_varQty, 	EE_ex_get_variablesQty());
	THW_printf("Page Active            : %d\t(0x%08X)  \r\n", 		EE_ex_get_activePage(), 	EE_ex_get_activePageAddress());
	THW_printf("pagesQty               : %d  (from %d to %d)(from 0x%08X to 0x%08X)\r\n",
			EE_ex_get_pagesQty(),
			EE_ex_get_startPage(), EE_ex_get_endPage(),
			EE_ex_get_startEepromAddress(), EE_ex_get_endEepromAddress());
	THW_printf("nbMaxElementsByPage    : %d  \r\n", 				EE_ex_get_nbMaxElementsByPage());
	THW_printf("nbMaxWrittenElements   : %d  \r\n", 				EE_ex_get_nbMaxWrittenElements());


	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_displayActionMenu(thw_drv_vee2_menuTab, thw_drv_vee2_menuTabSize);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_vee2_menuTabSize)){
		if(thw_drv_vee2_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_vee2_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		thw_main_setActive();	// Return to the previous menu
	}
}




//==============================================================================
//									VEE TESTS
//==============================================================================

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_writeAndVerifyNPacks(uint16_t packQty, bool executeCleanup)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_writeAndVerify()
{
	uint32_t 		activePage_old = 0;
	uint32_t 		tickStart = 0;
	uint32_t 		time = 0;
	uint32_t Index = 1;

	// Get VEE Page before write
	activePage_old = EE_ex_get_activePage();
	THW_goto(THW_DRV_VEE2_OFFSET_DISPLAY + thw_drv_vee2_menuTabSize, 0);
	THW_clearEndOfScreen();

	// Start time watching
	tickStart = HAL_GetTick();

	  uint64_t tmp[2] = {0};
	  /* Store 10 values of all variables in EEPROM, ascending order */
	  for (VarValue96bits = 1; VarValue96bits <= 10; VarValue96bits++)
	  {
	    for (Index = 1; Index < (EE_ex_get_variablesQty()*2)+1; Index+=2)
	    {
	      /* Wait any cleanup is completed before accessing flash again */
	      while (ErasingOnGoing == 1)
	      {
	    	  osDelay(1);
	      }

	      tmp[0] = Index*VarValue96bits;
	      tmp[1] = (Index+1)*VarValue96bits;
	      thw_drv_vee2_ee_status = EE_WriteVariable96bits(VirtAddVarTab[Index-1], tmp);   //write line of 128 bits
	      thw_drv_vee2_ee_status|= EE_ReadVariable96bits(VirtAddVarTab[Index-1], &a_VarData96Tab[Index-1]);

	      if (Index*VarValue96bits != a_VarData96Tab[Index-1]){
				THW_printf(" ==> Error when writting [Index-1]\r\n");
	      }

	      if ((Index+1)*VarValue96bits != a_VarData96Tab[Index]) {
				THW_printf(" ==> Error when writting [Index]\r\n");
	      }

	      /* Start cleanup IT mode, if cleanup is needed */
	      if ((thw_drv_vee2_ee_status & EE_STATUSMASK_CLEANUP) == EE_STATUSMASK_CLEANUP) {
	    	  ErasingOnGoing = 1;
	    	  thw_drv_vee2_ee_status|= EE_CleanUp_IT();
	      }

	      if ((thw_drv_vee2_ee_status & EE_STATUSMASK_ERROR) == EE_STATUSMASK_ERROR) {
	    	  Error_Handler();
	      }
	    }
	  }


	  /* Read all the variables */
	  for (Index = 1; Index < EE_ex_get_variablesQty()+1; Index+=2)
	  {
	    thw_drv_vee2_ee_status = EE_ReadVariable96bits(VirtAddVarTab[Index-1], tmp);
	    if (tmp[0] != a_VarData96Tab[Index-1]) {
			THW_printf(" ==> Error when reading [Index - 1]\r\n");
	    }

	    if (tmp[1] != a_VarData96Tab[Index]) {
			THW_printf(" ==> Error when reading [Index]\r\n");
	    }

	    if (thw_drv_vee2_ee_status != EE_OK) {
			THW_printf(" ==> Error when reading Status\r\n");
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
/// \fn 		void thw_drv_vee2_veeCleanup(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_veeCleanup(void)
{
	uint32_t time,tickStart = 0;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE2_OFFSET_DISPLAY + thw_drv_vee2_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Clean Vee
	ErasingOnGoing = 1;
	EE_CleanUp_IT();// Compute time
    while (ErasingOnGoing == 1)
    {
  	  osDelay(1);
    }
	time = (HAL_GetTick() - tickStart);

	THW_printf("Cleanup in %d ms      \r\n", time);

	// Display VEE Page (Old --> New)
	THW_printf("VEE active Page:  %2d      \r\n", EE_ex_get_activePage());
	THW_avoidClearScreen();
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_vee2_veeFormat(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_vee2_veeFormat()
{
	uint32_t activePage_old = 0;
	uint32_t time,tickStart = 0 ;

	THW_clearEndOfScreen();
	THW_goto(THW_DRV_VEE2_OFFSET_DISPLAY + thw_drv_vee2_menuTabSize, 0);

	// Start time watching
	tickStart = HAL_GetTick();

	// Format Vee
	activePage_old = EE_ex_get_activePage();
	EE_Status status = EE_Format(EE_FORCED_ERASE);
	// Compute time
	time = (HAL_GetTick() - tickStart);

	if(status != EE_OK){
		THW_printf("Format ERROR\r\n");
	}else{
		THW_printf("Format in %d ms\r\n", time);
		THW_printf("VEE active Page:  %2d --> %2d      \r\n", activePage_old, EE_ex_get_activePage());
	}
	THW_avoidClearScreen();

}

static void MPU_Config(void)
{
  /* MPU registers address definition */
  volatile uint32_t *mpu_type = (void *)0xE000ED90;
  volatile uint32_t *mpu_ctrl = (void *)0xE000ED94;
  volatile uint32_t *mpu_rnr = (void *)0xE000ED98;
  volatile uint32_t *mpu_rbar = (void *)0xE000ED9C;
  volatile uint32_t *mpu_rlar = (void *)0xE000EDA0;
  volatile uint32_t *mpu_mair0 = (void *)0xE000EDC0;

  /* Check that MPU is implemented and recover the number of regions available */
  uint32_t mpu_regions_nb = ((*mpu_type)>>8) & 0xff;

  /* If the MPU is implemented */
  if(mpu_regions_nb != 0)
  {
    /* Set RNR to configure the region with the highest number which also has the highest priority */
    *mpu_rnr = (mpu_regions_nb-1) & 0x000000FF;

    /* Set RBAR to get the region configured starting at FLASH_USER_START_ADDR, being non-shareable, r/w by any privilege level, and non executable */
    *mpu_rbar &= 0x00000000;
    *mpu_rbar = (EE_ex_get_startPage() | (0x0 << 3) | (0x1 << 1) | 0x1);

    /* Set RLAR to get the region configured ending at FLASH_USER_END_ADDR, being associated to the Attribute Index 0 and enabled */
    *mpu_rlar &= 0x00000000;
    *mpu_rlar = (EE_ex_get_endPage() | (0x0 << 1) | 0x1);

    /* Set MAIR0 so that the region configured is inner and outer non-cacheable */
    *mpu_mair0 &= 0xFFFFFF00;
    *mpu_mair0 |= ((0x4 << 4) | 0x4);

    /* Enable MPU + PRIVDEFENA=1 for the MPU rules to be effective + HFNMIENA=0 to ease debug */
    *mpu_ctrl &= 0xFFFFFFF8;
    *mpu_ctrl |= 0x5;
  }

  return;
}

/**
  * @brief  FLASH end of operation interrupt callback.
  * @param  ReturnValue: The value saved in this parameter depends on the ongoing procedure
  *                  Mass Erase: Bank number which has been requested to erase
  *                  Page Erase: Page which has been erased
  *                    (if 0xFFFFFFFF, it means that all the selected pages have been erased)
  *                  Program: Address which was selected for data program
  * @retval None
  */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* Call CleanUp callback when all requested pages have been erased */
  if (ReturnValue == 0xFFFFFFFF)
  {
    EE_EndOfCleanup_UserCallback();
  }
}

/**
  * @brief  Clean Up end of operation interrupt callback.
  * @param  None
  * @retval None
  */
void EE_EndOfCleanup_UserCallback(void)
{
  ErasingOnGoing = 0;
}

/**
  * @brief  Programmable Voltage Detector (PVD) Configuration
  *         PVD set to level 6 for a threshold around 2.9V.
  * @param  None
  * @retval None
  */
static void PVD_Config(void)
{
  PWR_PVDTypeDef sConfigPVD;
  sConfigPVD.PVDLevel = PWR_PVDLEVEL_6;
  sConfigPVD.Mode     = PWR_PVD_MODE_IT_RISING;
  if (HAL_PWR_ConfigPVD(&sConfigPVD) != HAL_OK) {Error_Handler();}

  /* Enable PVD */
  HAL_PWR_EnablePVD();

  //TODO: Set PVD NVIC
//  /* Enable and set PVD Interrupt priority */
//  HAL_NVIC_SetPriority(PVD_AVD_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(PVD_AVD_IRQn);
}
