/*
 * bal_struct.h
 *
 *  Created on: Aug 30, 2024
 *      Author: apajadon
 */

#ifndef BAL_CORE_BAL_STRUCT_H_
#define BAL_CORE_BAL_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// 									LED
//-----------------------------------------------------------------------------

typedef enum{
	bal_ledState_off = 0,
	bal_ledState_on,
	bal_ledState_slowBlinking,
	bal_ledState_fastBlinking,
}bal_ledState_t;

typedef struct
{
	uint8_t				ledId;
	char 				name[20];
	bal_ledState_t 		ledState;
	GPIO_PinState 		pinState;
	uint8_t				fastBlinkingCmp;
	uint8_t				slowBlinkingCmp;
	GPIO_TypeDef* 		GPIO_port;
	uint16_t 			GPIO_Pin;
	bool				inverted;
}bal_led_t;



//-----------------------------------------------------------------------------
// 									BTN
//-----------------------------------------------------------------------------
typedef enum{
	bal_btnState_off,
	bal_btnState_on
}bal_btnState_t;

typedef struct{
	void	(*startPress_cb)(void);
	void	(*shortPress_cb)(void);
	void	(*longPress_cb)(void);
	void	(*longPressInProgress_cb)(void);
}bal_btnCb_t;

typedef struct
{
	uint8_t			btnId;
	char 			name[20];
	GPIO_TypeDef* 	port;
	uint16_t 		pin;
	bool			inverted;
	GPIO_PinState 	actualPinState;
	GPIO_PinState 	oldPinState;
	uint16_t		pressCpt;
	bal_btnCb_t		cb;
}bal_btn_t;


#ifdef __cplusplus
}
#endif



#endif /* BAL_CORE_BAL_STRUCT_H_ */
