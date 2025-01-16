/*
 * pmgr_wrapper.h
 *
 *  Created on: Jan 6, 2025
 *      Author: apajadon
 */

#ifndef USERINTERFACES_POTARMANAGER_PMGR_WRAPPER_H_
#define USERINTERFACES_POTARMANAGER_PMGR_WRAPPER_H_



#include <stm32u5xx.h>


#ifdef __cplusplus
extern "C" {
#endif

// Wrapper functions
HAL_StatusTypeDef PMGR_Init(void);
HAL_StatusTypeDef PMGR_Exit(void);

#ifdef __cplusplus
}
#endif


#endif /* USERINTERFACES_POTARMANAGER_PMGR_WRAPPER_H_ */
