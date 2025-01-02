/*
 * CallBacks.c
 *
 *  Created on: Nov 22, 2024
 *      Author: apajadon
 */
#include <stm32u5xx.h>
#include <UserInterfaces/drvAdc/drvAdc.h>



//=============================================================================
//								ADC CALLBACK
//=============================================================================
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc)
{
	if (hadc->Instance == ADC1 || hadc->Instance == ADC2 || hadc->Instance == ADC4)
	{
		drvAdc_ConvCpltCallback(hadc->Instance);
	}
}

