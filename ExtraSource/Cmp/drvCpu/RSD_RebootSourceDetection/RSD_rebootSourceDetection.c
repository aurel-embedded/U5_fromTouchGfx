/*
 * RSD_rebootSourceDetection.c
 *
 *  Created on: 29 nov. 2022
 *      Author: apajadon
 */
#include "main.h"
#include <drvCpu/RSD_RebootSourceDetection/RSD_rebootSourceDetection_api.h>
#include "RSD_resetSource.h"
#include <stdbool.h>
#include <stm32u5xx_ll_rcc.h>

static rsd_resetSource_e rsd_resetsource = rsd_resetSource__NoReset;
static uint16_t resetState = 0x0000;

//------------------------------------------------------------------------------
/// \fn 		void RSD_Init(void)
/// \brief		detect the source of the reboot, must be used as soon as possible
///				According to Datasheet p.341
//------------------------------------------------------------------------------
void RSD_Init(void)
{

    // Comming from WWDG1?
	if(LL_RCC_IsActiveFlag_WWDGRST() 		&& LL_RCC_IsActiveFlag_PINRST()) 	rsd_resetsource = rsd_resetSource__Watchdog;
	else if(LL_RCC_IsActiveFlag_IWDGRST() 	&& LL_RCC_IsActiveFlag_PINRST())	rsd_resetsource = rsd_resetSource__IndependentWatchdog;
	else if(LL_RCC_IsActiveFlag_SFTRST() 	&& LL_RCC_IsActiveFlag_PINRST())	rsd_resetsource = rsd_resetSource__Software;
	else if(LL_RCC_IsActiveFlag_LPWRRST() 	&& LL_RCC_IsActiveFlag_PINRST())	rsd_resetsource = rsd_resetSource__LowPower;
	else if(LL_RCC_IsActiveFlag_BORRST() 	&& LL_RCC_IsActiveFlag_PINRST())	rsd_resetsource = rsd_resetSource__BOR;
	else if(LL_RCC_IsActiveFlag_LPWRRST() 	&& LL_RCC_IsActiveFlag_PINRST())	rsd_resetsource = rsd_resetSource__LowPower;
	else if(LL_RCC_IsActiveFlag_PINRST())										rsd_resetsource = rsd_resetSource__Pin;
	else																		rsd_resetsource = rsd_resetSource__NoReset;

	// Store & Clear Reset Source Flag
	resetState = RCC->CSR >> 16;
	LL_RCC_ClearResetFlags();
}


//------------------------------------------------------------------------------
/// \fn 		rsd_resetSource_e RSD_getResetSource(void)
/// \brief		Get the Reset Source
//------------------------------------------------------------------------------
rsd_resetSource_e RSD_getResetSource(void)
{
    return rsd_resetsource;
}

//------------------------------------------------------------------------------
/// \fn 		rsd_resetSource_e RSD_getResetState(void)
/// \brief		Get the Reset Source
//------------------------------------------------------------------------------
uint16_t RSD_getResetState(void)
{
    return resetState;
}

//==============================================================================
//==============================================================================
//								Local Function
//==============================================================================
//==============================================================================

