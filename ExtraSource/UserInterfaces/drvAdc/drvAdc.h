/*
 * drvAdc.h
 *
 *  Created on: Nov 22, 2024
 *      Author: apajadon
 */

#ifndef APPLICATION_USER_CMP_DRVADC_H_
#define APPLICATION_USER_CMP_DRVADC_H_
#include <cmsis_os2.h>
#include <stm32u5g9xx.h>
#include <UserInterfaces/drvAdc/drvAdc_values.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void 		drvAdc_ConvCpltCallback(ADC_TypeDef *Instance);
extern osStatus_t 	DRVADC_init(void);
extern osStatus_t 	DRVADC_getAdcValues(drvAdc_values_t *pValues);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_USER_CMP_DRVADC_H_ */
