/*
 * bal_config.c
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */
#include <UserInterfaces/BAL/bal_config.h>
#include <subModule/BAL_Core/bal_struct.h>
#include <main.h>



//-------------------------------------
// 				LED
//-------------------------------------
bal_led_t bal_ledList[] =
{
	// Led 2
	{
			.ledId = 			bal_led_Green,
			.name = 			"Led_Green",
			.GPIO_port = 		USER_LD3_GREEN_GPIO_Port,
			.GPIO_Pin = 		USER_LD3_GREEN_Pin,
			.inverted = 		true,
			.ledState = 		bal_ledState_fastBlinking,
	},
	// Led 3
	{
			.ledId = 			bal_led_Red,
			.name = 			"Led_Red",
			.GPIO_port = 		USER_LD2_RED_GPIO_Port,
			.GPIO_Pin = 		USER_LD2_RED_Pin,
			.inverted = 		true,
			.ledState = 		bal_ledState_slowBlinking,
	},
};
uint8_t bal_ledList_size = sizeof(bal_ledList) / sizeof(bal_led_t);


//-------------------------------------
// 				BTN
//-------------------------------------
bal_btn_t bal_btnList[] =
{

};
uint8_t bal_btnList_size = sizeof(bal_btnList) / sizeof(bal_btn_t);


