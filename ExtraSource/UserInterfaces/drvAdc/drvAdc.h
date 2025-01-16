/*
 * drvAdc.h
 *
 *  Created on: Nov 22, 2024
 *      Author: apajadon
 */

#ifndef APPLICATION_USER_CMP_DRVADC_H_
#define APPLICATION_USER_CMP_DRVADC_H_
#include <cmsis_os2.h>
#include <main.h>
#include <Tools/UserTypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void 				drvAdc_ConvCpltCallback(ADC_TypeDef *Instance);
extern osStatus_t 			DRVADC_init(void);
extern osStatus_t 			DRVADC_exit(void);
extern HAL_StatusTypeDef 	DRVADC_startAdc();
extern HAL_StatusTypeDef 	DRVADC_stopAdc();
extern osStatus_t 			DRVADC_getAdcValues(userTypes_6Uint16_t *pValues);
extern bool 				DRVADC_isAdcRunning(void);

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_USER_CMP_DRVADC_H_ */
