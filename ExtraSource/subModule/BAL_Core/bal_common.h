/*
 * bal_types.h
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */

#ifndef SRC_BAL_BAL_COMMON_H_
#define SRC_BAL_BAL_COMMON_H_

#include <stdbool.h>
#include <stdint.h>
#include <UserInterfaces/BAL/bal_config.h>
#include <subModule/BAL_Core/bal_struct.h>


#ifdef __cplusplus
extern "C" {
#endif



//-----------------------------------------------------------------------------
// 									BTN
//-----------------------------------------------------------------------------

extern void bal_btn_shortLongPress_init(	bal_btn_t 		*pBtnDef);
extern void bal_btn_shortLongPress_manage(	bal_btn_t 		*pBtnDef);

//-----------------------------------------------------------------------------
// 									LED
//-----------------------------------------------------------------------------
extern uint8_t BAL_ledState_TabName[][14];

extern void balCore_Led_init(	bal_led_t 	*pLedDef);
extern void balCore_Led_deInit(	bal_led_t 	*pLedDef);
extern void balCore_Led_manage(	bal_led_t 	*pLedDef);

#ifdef __cplusplus
}
#endif

#endif /* SRC_BAL_BAL_COMMON_H_ */
