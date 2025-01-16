
/*
 * bal_core.c
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */

#include <main.h>
#include <UserInterfaces/BAL/bal_config.h>
#include <subModule/BAL_Core/bal_common.h>
#include <subModule/BAL_Core/bal_struct.h>

// Clavier - Bouton
#define	BAL_CORE_BTN_PRESS_CPT_SHORT 1	// 1 * BAL_PERIOD_MIN_IN_MS ms
#define	BAL_CORE_BTN_PRESS_CPT_LONG 40	// 40 * BAL_PERIOD_MIN_IN_MS ms

#define BAL_CORE_LED_SLOW_BLINKING_MAX 25	// 25 * BAL_PERIOD_MIN_IN_MS ms
#define BAL_CORE_LED_FAST_BLINKING_MAX 5	// 5 * BAL_PERIOD_MIN_IN_MS ms



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 									BTN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/******************************************************************************
 ** Function name:		bal_btn_shortLongPress_init()
 ** Descriptions:		initialisation m�canisme de gestion de l'appui court
 ** 					et long sur les boutons
 ******************************************************************************/
void bal_btn_shortLongPress_init(bal_btn_t 	*pBtnDef)
{
	// Initialisation
	pBtnDef->actualPinState = !HAL_GPIO_ReadPin(pBtnDef->port, pBtnDef->pin);
	pBtnDef->oldPinState = !HAL_GPIO_ReadPin(pBtnDef->port, pBtnDef->pin);
	pBtnDef->pressCpt = 0;
	return;
}


/******************************************************************************
 ** Function name:		bal_btn_shortLongPress_manage()
 ** Descriptions:		gestion de l'appui court et long sur les boutons
 ******************************************************************************/
void bal_btn_shortLongPress_manage(	bal_btn_t 	*pBtnDef)
{
	// Lecture Etat courant
	GPIO_PinState pinStateLoc  = HAL_GPIO_ReadPin(pBtnDef->port, pBtnDef->pin);

	// Gestion montage PullUp ou PullDown
	if(pBtnDef->inverted){
		pBtnDef->actualPinState = !pinStateLoc;
	}else{
		pBtnDef->actualPinState = pinStateLoc;
	}

	// Gestion Appui bouton Court/Long
	if(pBtnDef->actualPinState == GPIO_PIN_SET){
		if(pBtnDef->actualPinState == pBtnDef->oldPinState){
			(pBtnDef->pressCpt)++;
			// Prise en compte de l'�tat appui long du bouton afin de le signaler
			if(pBtnDef->pressCpt == BAL_CORE_BTN_PRESS_CPT_LONG){
				if(pBtnDef->cb.longPressInProgress_cb != NULL){
					(*(pBtnDef->cb.longPressInProgress_cb))();
				}
			}
		}else{
			pBtnDef->pressCpt = 0;
			if(pBtnDef->cb.startPress_cb != NULL){
			(*(pBtnDef->cb.startPress_cb))();
			}
		}
	}else{
		// On valide l'Etat du bouton au relacher
		if(pBtnDef->pressCpt < BAL_CORE_BTN_PRESS_CPT_SHORT){
			// On n'envoie pas d'�v�nement
		}
		// Short Press
		else if(pBtnDef->pressCpt < BAL_CORE_BTN_PRESS_CPT_LONG){
			if(pBtnDef->cb.shortPress_cb != NULL){
				(*(pBtnDef->cb.shortPress_cb))();
			}
		}
		// Long Press
		else{
			if(pBtnDef->cb.longPress_cb != NULL){
				(*(pBtnDef->cb.longPress_cb))();
			}
		}
		pBtnDef->pressCpt = 0;
	}
	pBtnDef->oldPinState = pBtnDef->actualPinState;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 									LED
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/******************************************************************************
 ** Function name:		bal_Led_init()
 ******************************************************************************/
void balCore_Led_init(bal_led_t *pLedDef)
{
	GPIO_InitTypeDef  gpio_init_structure;

	/* Configure the GPIO_LED pin */
	gpio_init_structure.Pin   = pLedDef->GPIO_Pin;
	gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull  = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(pLedDef->GPIO_port, &gpio_init_structure);

	pLedDef->fastBlinkingCmp = 0;
	pLedDef->slowBlinkingCmp = 0;
}


/******************************************************************************
 ** Function name:		bal_Led_deInit()
 ******************************************************************************/
void balCore_Led_deInit(bal_led_t *pLedDef)
{
	GPIO_InitTypeDef  gpio_init_structure;

	/* DeInit the GPIO_LED pin */
	gpio_init_structure.Pin = pLedDef->GPIO_Pin;

	/* Turn off LED */
	HAL_GPIO_WritePin(pLedDef->GPIO_port, pLedDef->GPIO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_DeInit(pLedDef->GPIO_port, gpio_init_structure.Pin);

	pLedDef->ledState = bal_ledState_off;
	pLedDef->pinState = GPIO_PIN_RESET;
	pLedDef->fastBlinkingCmp = 0;
	pLedDef->slowBlinkingCmp = 0;
}


/******************************************************************************
 ** Function name:		bal_Led_manage()
 ******************************************************************************/
void balCore_Led_manage(bal_led_t 	*pLedDef)
{
	switch(pLedDef->ledState){
	case bal_ledState_off:
		pLedDef->pinState = (pLedDef->inverted)? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(pLedDef->GPIO_port, pLedDef->GPIO_Pin, pLedDef->pinState);
		pLedDef->fastBlinkingCmp = 0;
		pLedDef->slowBlinkingCmp = 0;
		break;

	case bal_ledState_fastBlinking:
		if(pLedDef->fastBlinkingCmp >= BAL_CORE_LED_FAST_BLINKING_MAX){
			pLedDef->pinState = !pLedDef->pinState;
			HAL_GPIO_WritePin(pLedDef->GPIO_port, pLedDef->GPIO_Pin, pLedDef->pinState);
			pLedDef->fastBlinkingCmp = 0;
		}else{
			pLedDef->fastBlinkingCmp++;
		}
		break;

	case bal_ledState_slowBlinking:
		if(pLedDef->slowBlinkingCmp >= BAL_CORE_LED_SLOW_BLINKING_MAX){
			pLedDef->pinState = !pLedDef->pinState;
			HAL_GPIO_WritePin(pLedDef->GPIO_port, pLedDef->GPIO_Pin, pLedDef->pinState);
			pLedDef->slowBlinkingCmp = 0;
		}else{
			(pLedDef->slowBlinkingCmp)++;
		}
		break;

	case bal_ledState_on:
		pLedDef->pinState = (pLedDef->inverted)? GPIO_PIN_RESET : GPIO_PIN_SET;
		HAL_GPIO_WritePin(pLedDef->GPIO_port, pLedDef->GPIO_Pin, pLedDef->pinState);
		pLedDef->fastBlinkingCmp = 0;
		pLedDef->slowBlinkingCmp = 0;
		break;

	default:
		pLedDef->fastBlinkingCmp = 0;
		pLedDef->slowBlinkingCmp = 0;
		break;

	}
}
