/*
 * RSD_rebootSourceDetection_api.h
 *
 *  Created on: 29 nov. 2022
 *      Author: apajadon
 */

#include "RSD_resetSource.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef USER_RSD_REBOOTSOURCEDETECTION_RSD_REBOOTSOURCEDETECTION_API_H_
#define USER_RSD_REBOOTSOURCEDETECTION_RSD_REBOOTSOURCEDETECTION_API_H_

extern void 				RSD_Init(void);
extern rsd_resetSource_e 	RSD_getResetSource(void);
extern uint16_t			 	RSD_getResetState(void);


#endif /* USER_RSD_REBOOTSOURCEDETECTION_RSD_REBOOTSOURCEDETECTION_API_H_ */
