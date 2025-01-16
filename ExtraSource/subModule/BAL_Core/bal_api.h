/*
 * bal.h
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */

#ifndef SRC_BAL_BAL_API_H_
#define SRC_BAL_BAL_API_H_
#include <stm32u5xx.h>
#include <UserInterfaces/BAL/bal_config.h>
#include <Submodule/BAL_Core/bal_struct.h>
#include <subModule/cmp.h>

#ifdef __cplusplus
extern "C" {
#endif



extern uint8_t BAL_ledState_TabName[4][14];

//=============================================================================
//=============================================================================
//							EXPORTED FUNCTIONS
//=============================================================================
//=============================================================================


extern HAL_StatusTypeDef 	BAL_init(	bal_led_t 	*ledList, uint8_t ledList_size,
										bal_btn_t 	*btnList, uint8_t btnList_size);
extern HAL_StatusTypeDef 	BAL_exit(void);
extern cmp_status_t 		BAL_getCmpStatus(void);

extern HAL_StatusTypeDef 	BAL_setLedStatus(uint8_t ledId, bal_ledState_t ledState);
extern HAL_StatusTypeDef 	BAL_getLedStatus(bal_ledId_e ledId, bal_ledState_t * pState);

extern HAL_StatusTypeDef 	BAL_getButtonStatus(uint8_t btnId, bal_btnState_t * pState);

// Button Callback
typedef enum{
	BAL_btnCbType_startPress = 0,
	BAL_btnCbType_shortPress,
	BAL_btnCbType_longPress,
	BAL_btnCbType_longPressInProgress,
}BAL_btnCbType_t;

extern HAL_StatusTypeDef 	BAL_registerBtnCb(uint8_t btn, BAL_btnCbType_t CbType, void(*pCb)(void));
extern HAL_StatusTypeDef 	BAL_unregisterBtnCb(uint8_t btn, BAL_btnCbType_t CbType);
extern HAL_StatusTypeDef 	BAL_unregisterBtnAllCb(uint8_t btn);

#ifdef __cplusplus
}
#endif

#endif /* SRC_BAL_BAL_API_H_ */
