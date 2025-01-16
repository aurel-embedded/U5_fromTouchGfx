/*
 * bal_config.h
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */

#ifndef SRC_BAL_BAL_CONFIG_H_
#define SRC_BAL_BAL_CONFIG_H_

#include <main.h>
#include <subModule/BAL_Core/bal_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------
// 				LED
//-------------------------------------
typedef enum{
	bal_led_Green = 1,
	bal_led_Red,
}bal_ledId_e;

extern bal_led_t 	bal_ledList[];
extern uint8_t 		bal_ledList_size;

//-------------------------------------
// 				BTN
//-------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* SRC_BAL_BAL_CONFIG_H_ */
