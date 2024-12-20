/*
 * mdi_wrapper.h
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 */

#ifndef MDI_MIDI_MDI_WRAPPER_H_
#define MDI_MIDI_MDI_WRAPPER_H_
#include <stm32u5xx.h>


#ifdef __cplusplus
extern "C" {
#endif

// Wrapper functions
HAL_StatusTypeDef MDI_Init(void);
HAL_StatusTypeDef MDI_Exit(void);

#ifdef __cplusplus
}
#endif


#endif /* MDI_MIDI_MDI_WRAPPER_H_ */
