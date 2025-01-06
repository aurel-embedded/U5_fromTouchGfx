/*
 * drvAdc_values.h
 *
 *  Created on: Nov 25, 2024
 *      Author: apajadon
 */

#ifndef APPLICATION_USER_CUSTOMSOURCES_USERINTERFACES_DRVADC_DRVADC_VALUES_H_
#define APPLICATION_USER_CUSTOMSOURCES_USERINTERFACES_DRVADC_DRVADC_VALUES_H_

#include <cmsis_os2.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint16_t 		potar1;
	uint16_t 		potar2;
	uint16_t 		potar3;
	uint16_t 		potar4;
	uint16_t 		potar5;
	uint16_t 		potar6;
}drvAdc_values_t;

#ifdef __cplusplus
}
#endif

#endif /* APPLICATION_USER_CUSTOMSOURCES_USERINTERFACES_DRVADC_DRVADC_VALUES_H_ */
