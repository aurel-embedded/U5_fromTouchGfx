/*
 * drvAdc.c
 *
 *  Created on: Nov 22, 2024
 *      Author: apajadon
 */
#include <cmsis_os2.h>
#include <Config/task_config.h>
#include <stm32u5xx.h>
#include <UserInterfaces/drvAdc/drvAdc.h>
#include <string.h>
#include <Tools/Tools.h>
#include <UserInterfaces/drvAdc/drvAdc_common.h>

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc4;
extern TIM_HandleTypeDef htim15;

//-----------------------------------------------------------------------------
// Task
//-----------------------------------------------------------------------------
#define DRVADC_TASK_DELAY 20
osThreadId_t drvAdc_TaskHandle;
const osThreadAttr_t drvAdc_Task_attributes = {
		.name = "ADC tsk",
		.stack_size = TSK_CFG__STACK__TSK_ADC,
		.priority = TSK_CFG__PRIO__TSK_ADC,
};
static void drvAdc_Task_fn(void *argument);

//-----------------------------------------------------------------------------
// CHANNEL ADC QTY
//-----------------------------------------------------------------------------
#define DRVADC_CHANNEL_QTY_ADC1	2
#define DRVADC_CHANNEL_QTY_ADC2	2
#define DRVADC_CHANNEL_QTY_ADC4	2

//-----------------------------------------------------------------------------
// DMA BUFFER
//-----------------------------------------------------------------------------
uint16_t drvAdc_dmaBuf_ADC1[DRVADC_CHANNEL_QTY_ADC1] __ALIGNED(32);
uint16_t drvAdc_dmaBuf_ADC2[DRVADC_CHANNEL_QTY_ADC2] __ALIGNED(32);
uint16_t drvAdc_dmaBuf_ADC4[DRVADC_CHANNEL_QTY_ADC4] __ALIGNED(32);

//-----------------------------------------------------------------------------
// CALLBACK BUFFER - Double Buffer
//-----------------------------------------------------------------------------
uint8_t drvAdc_callBackBuf_ADC1[2][DRVADC_CHANNEL_QTY_ADC1];
uint8_t drvAdc_callBackBuf_ADC2[2][DRVADC_CHANNEL_QTY_ADC2];
uint8_t drvAdc_callBackBuf_ADC4[2][DRVADC_CHANNEL_QTY_ADC4];
volatile uint8_t drvAdc_callBackBuf_activeInd_ADC1 = 0; // Index du buffer actif
volatile uint8_t drvAdc_callBackBuf_activeInd_ADC2 = 0; // Index du buffer actif
volatile uint8_t drvAdc_callBackBuf_activeInd_ADC4 = 0; // Index du buffer actif

//-----------------------------------------------------------------------------
// MEMORY SHARED
//-----------------------------------------------------------------------------
typedef struct{
	drvAdc_values_t 	values;
	osMutexId_t  		mtx_id;
}drvAdc_memoryShared_t;
drvAdc_memoryShared_t drvAdc_memoryShared;

const osMutexAttr_t drvAdc_memoryShared_mtx_attr = {
		"drvAdc_mtx",                             	// human readable mutex name
		osMutexRecursive | osMutexPrioInherit,  	// attr_bits
		NULL,                                    	// memory for control block
		0U                                       	// size for control block
};


uint16_t adcVal = 0;
int converted_value = 0;

//=============================================================================
//									CALLBACK
//=============================================================================
void drvAdc_ConvCpltCallback(ADC_TypeDef *Instance)
{
	volatile uint8_t *pActiveInd = NULL;
	uint8_t maxChannelQty = 0;
	uint8_t *pCallBackBuf = NULL;
	uint16_t *pDmaBuf = NULL;

	if (Instance == ADC1){
		pActiveInd = &drvAdc_callBackBuf_activeInd_ADC1;
		maxChannelQty = DRVADC_CHANNEL_QTY_ADC1;
		pDmaBuf = drvAdc_dmaBuf_ADC1;
		pCallBackBuf = drvAdc_callBackBuf_ADC1[*pActiveInd];
	}
	else if (Instance == ADC2){
		pActiveInd = &drvAdc_callBackBuf_activeInd_ADC2;
		maxChannelQty = DRVADC_CHANNEL_QTY_ADC2;
		pDmaBuf = drvAdc_dmaBuf_ADC2;
		pCallBackBuf = drvAdc_callBackBuf_ADC2[*pActiveInd];
	}
	else if (Instance == ADC4){
		pActiveInd = &drvAdc_callBackBuf_activeInd_ADC4;
		maxChannelQty = DRVADC_CHANNEL_QTY_ADC4;
		pDmaBuf = drvAdc_dmaBuf_ADC4;
		pCallBackBuf = drvAdc_callBackBuf_ADC4[*pActiveInd];
	}else{
		return;
	}

	for(uint8_t i = 0; i < maxChannelQty; i++){
		pCallBackBuf[i] = map(pDmaBuf[i], 0, 4095, 0, 100);
	}

	// Flip Flop the active buffer
	*pActiveInd = !*pActiveInd;
}

//=============================================================================
//								LOCAL FUNCTIONs
//=============================================================================




//=============================================================================
//								TASK FUNCTION
//=============================================================================
static void drvAdc_Task_fn(void *argument)
{
	// Warning: As we use os object in the ADC callback, we must
	//				start DMA & Timer here, once the os objects
	//				are created
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)drvAdc_dmaBuf_ADC1, 2) != HAL_OK){
		// Handle error (e.g., log or notify)
		return;
	}

	if(HAL_ADC_Start_DMA(&hadc2, (uint32_t *)drvAdc_dmaBuf_ADC2, 2) != HAL_OK){
		// Handle error (e.g., log or notify)
		return;
	}

	if(HAL_ADC_Start_DMA(&hadc4, (uint32_t *)drvAdc_dmaBuf_ADC4, 2) != HAL_OK){
		// Handle error (e.g., log or notify)
		return;
	}

    if (HAL_TIM_Base_Start(&htim15) != HAL_OK) {
        // Handle error (e.g., log or notify)
        return;
    }

	/* Infinite loop */
	for(;;)
	{
		// Read From unactiv CallBack Buffer

		// Get Memory shared Mutex
		if(osMutexAcquire(drvAdc_memoryShared.mtx_id, osWaitForever) == osOK){
			// Copy to memory Shared (using the unused buffer)
			drvAdc_memoryShared.values.potar1 = drvAdc_callBackBuf_ADC2[!drvAdc_callBackBuf_activeInd_ADC2][0];
			drvAdc_memoryShared.values.potar2 = drvAdc_callBackBuf_ADC2[!drvAdc_callBackBuf_activeInd_ADC2][1];
			drvAdc_memoryShared.values.potar3 = drvAdc_callBackBuf_ADC4[!drvAdc_callBackBuf_activeInd_ADC4][0];
			drvAdc_memoryShared.values.potar4 = drvAdc_callBackBuf_ADC4[!drvAdc_callBackBuf_activeInd_ADC4][1];
			drvAdc_memoryShared.values.potar5 = drvAdc_callBackBuf_ADC1[!drvAdc_callBackBuf_activeInd_ADC1][0];
			drvAdc_memoryShared.values.potar6 = drvAdc_callBackBuf_ADC1[!drvAdc_callBackBuf_activeInd_ADC1][1];

			// Release Mutex
			osMutexRelease(drvAdc_memoryShared.mtx_id);
		}


		osDelay(DRVADC_TASK_DELAY);
	}
	/* USER CODE END adcTask_fn */
}

//=============================================================================
//								API FUNCTIONs
//=============================================================================

osStatus_t DRVADC_init()
{
	drvAdc_memoryShared.mtx_id = osMutexNew(&drvAdc_memoryShared_mtx_attr);
	if(drvAdc_memoryShared.mtx_id == NULL)
		return osErrorParameter;

	drvAdc_TaskHandle = osThreadNew(drvAdc_Task_fn, NULL, &drvAdc_Task_attributes);
	if(drvAdc_TaskHandle == NULL)
		return osErrorResource;

	return osOK;
}


osStatus_t DRVADC_getAdcValues(drvAdc_values_t 	*pValues)
{
	// Check for invalid parameters
	if (pValues == NULL) {
		return osErrorParameter; // Invalid pointer
	}

	if(drvAdc_memoryShared.mtx_id == NULL){
		return osErrorResource;
	}

	if(osMutexAcquire(drvAdc_memoryShared.mtx_id, 10) != osOK){
		return osErrorTimeout;
	}

	memcpy(pValues, &(drvAdc_memoryShared.values), sizeof(drvAdc_values_t));

	osMutexRelease(drvAdc_memoryShared.mtx_id);

	return osOK;
}

