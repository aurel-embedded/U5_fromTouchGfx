/*
 * drvCpu_api.h
 *
 *  Created on: 31 janv. 2024
 *      Author: apajadon
 */

#ifndef DRVCPU_DRVCPU_API_H_
#define DRVCPU_DRVCPU_API_H_
#include <stdint.h>
#include "RSD_RebootSourceDetection/RSD_resetSource.h"

extern void 				DRVCPU_Init(void);
extern void 				DRVCPU_resetCpu(void);
extern rsd_resetSource_e 	DRVCPU_getResetSource(void);
extern uint16_t 			DRVCPU_getResetState(void);

extern void 				DrvCpu_BOR_init(void);
extern uint16_t 			DrvCpu_BOR_getLevel(void);

#endif /* DRVCPU_DRVCPU_API_H_ */
