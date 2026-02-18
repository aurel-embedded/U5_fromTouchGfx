/*
 * drvCpu.c
 *
 *  Created on: 31 janv. 2024
 *      Author: apajadon
 */
#include <APP.h>
#include "main.h"
#include <drvCpu/drvCpu_api.h>
#include "RSD_RebootSourceDetection/RSD_rebootSourceDetection_api.h"

static HAL_StatusTypeDef DrvCpu_BOR_Set(uint16_t BOR_level);

//------------------------------------------------------------------------------
/// \fn 		void DRVCPU_Init(void)
/// \brief		Initialize Cpu Driver
//------------------------------------------------------------------------------
void DRVCPU_Init(void)
{
	RSD_Init();

#ifdef DRVCPU_BOR_ENABLE
	DrvCpu_BOR_init();
#endif
}

//------------------------------------------------------------------------------
/// \fn 		void DRVCPU_resetCpu(void)
/// \brief		Reset the CPU
//------------------------------------------------------------------------------
void DRVCPU_resetCpu(void)
{
	NVIC_SystemReset();
}

//------------------------------------------------------------------------------
/// \fn 		uint8_t DRVCPU_getResetSource(void)
/// \brief		return the reset Source
//------------------------------------------------------------------------------
rsd_resetSource_e DRVCPU_getResetSource(void)
{
	return RSD_getResetSource();
}

//------------------------------------------------------------------------------
/// \fn 		uint16_t DRVCPU_getResetState(void)
/// \brief		return the reset State
//------------------------------------------------------------------------------
uint16_t DRVCPU_getResetState(void)
{
	return RSD_getResetState();
}


//************************************************************
void DrvCpu_BOR_init(void)
{
    // read Config
	if((DrvCpu_BOR_getLevel() != DRVCPU_BOR_LEVEL)) {
		// set BOR
		DrvCpu_BOR_Set(DRVCPU_BOR_LEVEL);
	}
}

//************************************************************
static HAL_StatusTypeDef DrvCpu_BOR_Set(uint16_t BOR_level)
{
	FLASH_OBProgramInitTypeDef FLASH_OBInitStruct;

	HAL_FLASH_Unlock();
	/* Unlock the option bytes block access */
	HAL_FLASH_OB_Unlock();

    HAL_FLASHEx_OBGetConfig(&FLASH_OBInitStruct);
	/* Select the desired V(BOR) Level */
	FLASH_OBInitStruct.OptionType = OPTIONBYTE_WRP;
	FLASH_OBInitStruct.USERType   = OB_USER_BOR_LEV;
	FLASH_OBInitStruct.USERConfig = BOR_level;
	HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct);

	/* Launch the option byte loading : Generate System Reset to load the new option byte values*/
	HAL_FLASH_OB_Launch();
	/* Locks the option bytes block access */
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Unlock();

	return HAL_OK;
}

//************************************************************
uint16_t DrvCpu_BOR_getLevel(void)
{
	// read Config
	FLASH_OBProgramInitTypeDef FLASH_OBInitStruct;
	FLASH_OBInitStruct.OptionType = OPTIONBYTE_WRP;
	FLASH_OBInitStruct.USERType   = OB_USER_BOR_LEV;
    HAL_FLASHEx_OBGetConfig(&FLASH_OBInitStruct);
    return  (FLASH_OBInitStruct.USERConfig);
}
