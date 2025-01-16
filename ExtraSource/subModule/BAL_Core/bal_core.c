/*
 * bal.c
 *
 *  Created on: 16 jan. 2023
 *      Author: apajadon
 */

#include <cmsis_os2.h>
#include <string.h>
#include <Config/task_config.h>
#include <subModule/BAL_Core/bal_api.h>
#include <subModule/BAL_Core/bal_common.h>
#include <subModule/BAL_Core/bal_struct.h>
#include <subModule/cmp.h>

#define BAL_COMPONENT_NAME "BAL\0"

#define BAL_PERIOD_MIN_IN_MS 20


//-----------------------------------------------------------------------------
// INTERNAL DATA
//-----------------------------------------------------------------------------

typedef struct{
	cmp_status_t cmpStatus;
	struct{
		struct{
			bal_led_t 	*list;
			uint8_t 	list_size;
		}led;
		struct{
			bal_btn_t 	*list;
			uint8_t 	list_size;
		}btn;
	}config;
}bal_internalData_t;
bal_internalData_t bal_internalData;

//-----------------------------------------------------------------------------
// THREAD
//-----------------------------------------------------------------------------
osThreadId_t bal_tsk_id;
const osThreadAttr_t bal_tsk_attr = {
		.name = "bal_tsk",
		.stack_size = TSK_CFG__STACK__TSK_BAL,
		.priority = TSK_CFG__PRIO__TSK_BAL,
};
static void bal_tsk_fn(void *arg);


static HAL_StatusTypeDef 		bal_Led_initAll(void);
static HAL_StatusTypeDef 		bal_Led_deInitAll(void);
static HAL_StatusTypeDef 		bal_Led_manageAll(void);
static HAL_StatusTypeDef 		bal_Btn_initAll(void);
static HAL_StatusTypeDef 		bal_Btn_deInitAll(void);
static HAL_StatusTypeDef 		bal_Btn_manageAll(void);

static bal_led_t* 	bal_led_findItem(uint8_t ledId);
static bal_btn_t* 	bal_btn_findItem(uint8_t btnId);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 									TASK
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void bal_tsk_fn(void  * arg)
{

	// Init Led
	bal_Led_initAll();

	// Init Bouton
	bal_Btn_initAll();

	// Component Status
	bal_internalData.cmpStatus.isTaskRunning = true;
	bal_internalData.cmpStatus.cmpMode = cmp_mode_nominal;

	while(bal_internalData.cmpStatus.isTaskRunning == true)
	{
		bal_Led_manageAll();	// Manage Led
		bal_Btn_manageAll();	// Manage button

		// Global task delay
		osDelay(BAL_PERIOD_MIN_IN_MS);

	}

	bal_Led_deInitAll();
	bal_Btn_deInitAll();

	osThreadTerminate(NULL);
}


/******************************************************************************
 ** Function name:		bal_Led_initAll()
 ** Descriptions:		initialisation mécanisme de gestion des leds
 ******************************************************************************/
static HAL_StatusTypeDef bal_Led_initAll(void)
{
	if(bal_internalData.config.led.list == NULL || bal_internalData.config.led.list_size == 0)
		return HAL_ERROR;

	for(uint8_t i = 0; i < bal_internalData.config.led.list_size; i++){
		balCore_Led_init(&(bal_internalData.config.led.list[i]));
	}

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		bal_Led_deInitAll()
 ** Descriptions:		déinitialisation mécanisme de gestion des leds
 ******************************************************************************/
static HAL_StatusTypeDef bal_Led_deInitAll(void)
{
	if(bal_internalData.config.led.list == NULL || bal_internalData.config.led.list_size == 0)
		return HAL_ERROR;

	for(uint8_t i = 0; i < bal_internalData.config.led.list_size; i++){
		balCore_Led_deInit(&(bal_internalData.config.led.list[i]));
	}

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		bal_Led_manageAll()
 ** Descriptions:		mécanisme de gestion des leds
 ******************************************************************************/
static HAL_StatusTypeDef bal_Led_manageAll(void)
{
	if(bal_internalData.config.led.list == NULL || bal_internalData.config.led.list_size == 0)
		return HAL_ERROR;

	for(uint8_t i = 0; i < bal_internalData.config.led.list_size; i++){
		balCore_Led_manage(&(bal_internalData.config.led.list[i]));
	}

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		bal_Btn_initAll()
 ** Descriptions:		initialisation mécanisme de gestion des boutons
 ******************************************************************************/
static HAL_StatusTypeDef bal_Btn_initAll(void)
{
	if(bal_internalData.config.btn.list == NULL || bal_internalData.config.btn.list_size == 0)
		return HAL_ERROR;

	for(uint8_t i = 0; i < bal_internalData.config.btn.list_size; i++){
		bal_btn_shortLongPress_init(&(bal_internalData.config.btn.list[i]));
	}

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		bal_Led_deInitAll()
 ** Descriptions:		déinitialisation mécanisme de gestion des boutons
 ******************************************************************************/
static HAL_StatusTypeDef bal_Btn_deInitAll(void)
{
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		bal_Btn_manageAll()
 ** Descriptions:		mécanisme de gestion des boutons
 ******************************************************************************/
static HAL_StatusTypeDef bal_Btn_manageAll(void)
{
	if(bal_internalData.config.btn.list == NULL || bal_internalData.config.btn.list_size == 0)
		return HAL_ERROR;

	for(uint8_t i = 0; i < bal_internalData.config.btn.list_size; i++){
		bal_btn_shortLongPress_manage(&(bal_internalData.config.btn.list[i]));
	}

	return HAL_OK;
}


//------------------------------------------------------------------------------
/// \fn          static bal_led_t* bal_led_findItem(bal_ledId_e ledId)
/// \brief       Finds an LED item in the LED list by its identifier.
///
/// This function searches through the list of LEDs (`bal_ledList`) to find the
/// LED with the specified identifier (`ledId`). If the LED is found, a pointer
/// to the corresponding `bal_led_t` structure is returned. If the LED is not
/// found, the function returns `NULL`.
///
/// \param[in]   ledId   The identifier of the LED to find in the list.
///
/// \return      A pointer to the `bal_led_t` structure corresponding to the
///              specified `ledId`, or `NULL` if the LED is not found.
///
/// \note        The function iterates over the `bal_ledList` array and compares
///              each element's `ledId` to the provided `ledId`.
//------------------------------------------------------------------------------
static bal_led_t* bal_led_findItem(uint8_t ledId)
{
	if(bal_internalData.config.led.list == NULL || bal_internalData.config.led.list_size == 0)
		return NULL;

	for(uint8_t i = 0; i < bal_internalData.config.led.list_size; i++)
	{
		if(bal_internalData.config.led.list[i].ledId == ledId)
			return &(bal_internalData.config.led.list[i]);
	}
	return NULL;
}

//------------------------------------------------------------------------------
/// \fn          static bal_btn_t* bal_btn_findItem(bal_btnId_e btnId)
/// \brief       Finds a button item in the button list by its identifier.
///
/// This function searches through the list of buttons (`bal_btnList`) to find
/// the button with the specified identifier (`btnId`). If the button is found,
/// a pointer to the corresponding `bal_btn_t` structure is returned. If the button
/// is not found, the function returns `NULL`.
///
/// \param[in]   btnId   The identifier of the button to find in the list.
///
/// \return      A pointer to the `bal_btn_t` structure corresponding to the
///              specified `btnId`, or `NULL` if the button is not found.
///
/// \note        The function iterates over the `bal_btnList` array and compares
///              each element's `btnId` to the provided `btnId`.
//------------------------------------------------------------------------------
static bal_btn_t* bal_btn_findItem(uint8_t btnId)
{
	if(bal_internalData.config.btn.list == NULL || bal_internalData.config.btn.list_size == 0)
		return NULL;

	for(uint8_t i = 0; i < bal_internalData.config.btn.list_size; i++)
	{
		if(bal_internalData.config.btn.list[i].btnId == btnId)
			return &(bal_internalData.config.btn.list[i]);
	}
	return NULL;
}


//=============================================================================
//=============================================================================
//							EXPORTED FUNCTIONS
//=============================================================================
//=============================================================================

/******************************************************************************
 ** Function name:		BAL_init
 ** Descriptions:		initialise le composant BAL
 ** parameters:			NA
 ** Returned value:		Status
 ******************************************************************************/
HAL_StatusTypeDef BAL_init(	bal_led_t 	*ledList, uint8_t ledList_size,
							bal_btn_t 	*btnList, uint8_t btnList_size)
{
	bal_internalData.config.led.list = 		ledList;
	bal_internalData.config.led.list_size = ledList_size;
	bal_internalData.config.btn.list = 		btnList;
	bal_internalData.config.btn.list_size = btnList_size;

	if(bal_internalData.cmpStatus.isTaskRunning == true)
	{
		return HAL_OK;
	}

	// Component Status
	bal_internalData.cmpStatus.isTaskRunning = false;
	bal_internalData.cmpStatus.cmpMode = cmp_mode_notDefined;

	// Creating Task
	bal_tsk_id = osThreadNew(bal_tsk_fn, NULL, &bal_tsk_attr);
	if(bal_tsk_id == NULL){
		return HAL_ERROR;
	}

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		BAL_exit
 ** Descriptions:		finalise le module BAL
 ** parameters:			None
 ** Returned value:		Status
 ******************************************************************************/
HAL_StatusTypeDef BAL_exit(void)
{
	bal_Led_deInitAll();

	// Delete task
	osThreadTerminate(bal_tsk_id);

	// Component Status
	bal_internalData.cmpStatus.isTaskRunning = false;
	bal_internalData.cmpStatus.cmpMode = cmp_mode_notDefined;

	return(HAL_OK);
}

//------------------------------------------------------------------------------
/// \fn 		cmp_status_t BAL_getCmpStatus(void)
/// \brief		return cmpstatus info
//------------------------------------------------------------------------------
cmp_status_t BAL_getCmpStatus(void)
{
	return bal_internalData.cmpStatus;
}

/******************************************************************************
 ** Function name:		BAL_setLedStatus(bal_ledName_e led, bal_ledState_t ledState)
 ** Descriptions:		definit l'état de la led User led Offset
 ******************************************************************************/
HAL_StatusTypeDef BAL_setLedStatus(bal_ledId_e ledId, bal_ledState_t ledState)
{
	// find Led
	bal_led_t *pItem = bal_led_findItem(ledId);
	if(pItem == NULL)
		return HAL_ERROR;

	// Set State
	pItem->ledState = ledState;

	return(HAL_OK);
}


/******************************************************************************
 ** Function name:		bal_ledState_t BAL_getLedStatus(bal_ledName_e led)
 ** Descriptions:		récupère l'état de la led User ledOffset
 ******************************************************************************/
HAL_StatusTypeDef BAL_getLedStatus(bal_ledId_e ledId, bal_ledState_t * pState)
{
	// find Led
	bal_led_t *pItem = bal_led_findItem(ledId);
	if(pItem == NULL)
		return HAL_ERROR;

	*pState = pItem->ledState;

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		BAL_registerBtnCb
 ** Descriptions:		definit la callBack pour l'action du bouton btn
 ******************************************************************************/
HAL_StatusTypeDef BAL_registerBtnCb(uint8_t btnId, BAL_btnCbType_t CbType, void (*pCb)(void))
{
	bal_btn_t * pItem = bal_btn_findItem(btnId);
	if(pItem == NULL){
		return HAL_ERROR;
	}

	switch (CbType) {
	case BAL_btnCbType_startPress:
		pItem->cb.startPress_cb = pCb;
		break;
	case BAL_btnCbType_shortPress:
		pItem->cb.shortPress_cb = pCb;
		break;
	case BAL_btnCbType_longPress:
		pItem->cb.longPress_cb = pCb;
		break;
	case BAL_btnCbType_longPressInProgress:
		pItem->cb.longPressInProgress_cb = pCb;
		break;
	default:
		break;
	}
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		BAL_unregisterBtnCb
 ** Descriptions:		reset la callBack pour l'action du bouton btn
 ******************************************************************************/
HAL_StatusTypeDef BAL_unregisterBtnCb(uint8_t btnId, BAL_btnCbType_t CbType)
{
	bal_btn_t * pItem = bal_btn_findItem(btnId);
	if(pItem == NULL){
		return HAL_ERROR;
	}

	switch (CbType) {
	case BAL_btnCbType_startPress:
		pItem->cb.startPress_cb = NULL;
		break;
	case BAL_btnCbType_shortPress:
		pItem->cb.shortPress_cb = NULL;
		break;
	case BAL_btnCbType_longPress:
		pItem->cb.longPress_cb = NULL;
		break;
	case BAL_btnCbType_longPressInProgress:
		pItem->cb.longPressInProgress_cb = NULL;
		break;
	default:
		break;
	}
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		BAL_unregisterBtnCb
 ** Descriptions:		reset toutes les callBack du bouton btn
 ******************************************************************************/
HAL_StatusTypeDef BAL_unregisterBtnAllCb(uint8_t btnId)
{
	bal_btn_t * pItem = bal_btn_findItem(btnId);
	if(pItem == NULL){
		return HAL_ERROR;
	}

	pItem->cb.startPress_cb = NULL;
	pItem->cb.shortPress_cb = NULL;
	pItem->cb.longPress_cb = NULL;
	pItem->cb.longPressInProgress_cb = NULL;
	return HAL_OK;
}


/******************************************************************************
 ** Function name:		bal_btnState_t BAL_getButtonStatus(bal_btnName_e btn)
 ** Descriptions:		récupère l'état de la touchet
 ******************************************************************************/
HAL_StatusTypeDef BAL_getButtonStatus(uint8_t btnId, bal_btnState_t * pState)
{
	bal_btn_t * pItem = bal_btn_findItem(btnId);
	if(pItem == NULL){
		return HAL_ERROR;
	}

	if(pItem->actualPinState == GPIO_PIN_SET) {
		*pState = bal_btnState_on;
	}else{
		*pState = bal_btnState_off;
	}

	return HAL_OK;
}









