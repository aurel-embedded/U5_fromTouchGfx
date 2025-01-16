/*
 * thw_com.c
 *
 *  Created on: 4 nov. 2022
 *      Author: apajadon
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stm32h7xx_ll_usart.h>
#include <string.h>
#include <thw_config.h>
#include <THW_core/THW_testHardware_common.h>
#include "main.h"

volatile uint8_t thw_line[RX_BUF_SIZE+1];
volatile uint8_t rx = 0;
UART_HandleTypeDef thw_uartHdl;

//-----------------------------------------------------------------------------
// Mutex
//-----------------------------------------------------------------------------
const osMutexAttr_t thw_com_mtx_attr = {
	"thw_com_mtx",                           // human readable mutex name
	osMutexRecursive | osMutexPrioInherit,  // attr_bits
	NULL,                                    // memory for control block
	0U                                       // size for control block
};
osMutexId_t  	thw_com_mtx_id;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//								LOCAL FUNCTION
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/******************************************************************************
 ** Function name:		thw_com_lock()
 ** Descriptions:		Lock le mutex de l'affichage DBG
 ******************************************************************************/
static void thw_com_lock(void)
{
	if(osKernelGetState() == osKernelRunning){
		if(thw_com_mtx_id != NULL){
			osMutexWait(thw_com_mtx_id, osWaitForever);
		}
	}
}
/******************************************************************************
 ** Function name:		thw_com_unlock()
 ** Descriptions:		Unloock le mutex de l'affichage DBG
 ******************************************************************************/
static void thw_com_unlock(void)
{
	if(osKernelGetState() == osKernelRunning){
		if(thw_com_mtx_id != NULL){
			osMutexRelease(thw_com_mtx_id);
		}
	}
}

/******************************************************************************
 ** Function name:		THW_comInit
 ** Descriptions:		THW component Init
 ******************************************************************************/
HAL_StatusTypeDef thw_com_init(void)
{
	thw_uartHdl.Instance            = THW_UART;
//	thw_uartHdl.Init.BaudRate       = 115200;
	thw_uartHdl.Init.BaudRate       = (115200 * 4);
	thw_uartHdl.Init.WordLength     = UART_WORDLENGTH_8B;
	thw_uartHdl.Init.StopBits       = UART_STOPBITS_1;
	thw_uartHdl.Init.Parity         = UART_PARITY_NONE;
	thw_uartHdl.Init.Mode           = UART_MODE_TX_RX;
	thw_uartHdl.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
	thw_uartHdl.Init.OverSampling   = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&thw_uartHdl) != HAL_OK)
	{
		/* Initialization Error */
		return HAL_ERROR;
	}

	// Create Mutex
	thw_com_mtx_id = osMutexNew(&thw_com_mtx_attr);
	if (thw_com_mtx_id == NULL)  {
		return HAL_ERROR;
	}


	/* USART1 interrupt Init */
//	HAL_NVIC_SetPriority(USARTx_COM_TEST_IRQn, UART_IT_PRIORITY, 0);
//	HAL_NVIC_EnableIRQ(USARTx_COM_TEST_IRQn);
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		thw_com_exit
 ** Descriptions:		THW component Exit
 ******************************************************************************/
HAL_StatusTypeDef thw_com_exit(void)
{
	// Create Mutex
	if (thw_com_mtx_id != NULL)  {
		osMutexDelete(thw_com_mtx_id);
	}

	HAL_UART_DeInit(&thw_uartHdl);

	return HAL_OK;
}


//-----------------------------------------------------------------------------
/**
 * @brief Lit une trame reçue du THW.
 * @param frame pointeur sur la trame lu.
 * @return 1: trame disponible; 0: timeout
 */
//-----------------------------------------------------------------------------
bool thw_com_manageRx(void)
{
	bool toReturn = false;
	static int rx_index = 0;
	static char rx_buffer[RX_BUF_SIZE];   // Local holding buffer to

//	if (HAL_UART_Receive(&thw_uartHdl, (uint8_t*)&rx, 1, 5) == HAL_OK)
	if (HAL_UART_Receive(&thw_uartHdl, (uint8_t*)&rx, 1, 1) == HAL_OK)
	{
		HAL_UART_Transmit(&thw_uartHdl, (uint8_t*)&rx, 1, 5); // Send it back for Screen Display
		if ((rx == '\r') || (rx == '\n')) // Is this an end-of-line condition, either will suffice?
		{
			if (rx_index != 0) // Line has some content
			{
				memcpy((void *)thw_line, rx_buffer, rx_index); // Copy to static line buffer from dynamic receive buffer
				thw_line[rx_index] = 0; // Add terminating NUL
				rx_index = 0; // Reset content pointer
				toReturn = true; // flag new line valid for processing
			}
		}
		else
		{
			if (rx_index == RX_BUF_SIZE) // If resync or overflows pull back to start
				rx_index = 0;

			rx_buffer[rx_index++] = rx; // Copy to buffer and increment
		}
	}
	return toReturn;
}

#define THW_COM_MAX_FORMAT_LENGTH 256
char thw_comfmt[THW_COM_MAX_FORMAT_LENGTH];



/******************************************************************************
 ** Function name:		THW_comTransmit
 ** Descriptions:		Transmission
 ******************************************************************************/
uint16_t thw_com_transmit(uint8_t *data, uint16_t len)
{
	uint16_t ubSend = 0;
	while (ubSend < len)
	{
		/* Wait for TXE flag to be raised */
		while (!LL_USART_IsActiveFlag_TXE(thw_uartHdl.Instance)){
			osDelay(1);
		}

		/* If last char to be sent, clear TC flag */
		if (ubSend == (len - 1)){
			LL_USART_ClearFlag_TC(thw_uartHdl.Instance);
		}

		/* Write character in Transmit Data register.
	       TXE flag is cleared by writing data in TDR register */
		LL_USART_TransmitData8(thw_uartHdl.Instance, *(data + ubSend++));
	}

	/* Wait for TC flag to be raised for last char */
	while (!LL_USART_IsActiveFlag_TC(thw_uartHdl.Instance)){
		osDelay(1);
	}

	return len;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//								EXTERNAL FUNCTION
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/******************************************************************************
 ** Function name:		THW_COM_printf()
 ** Descriptions:		Envoi asysnchrone sur la console d'affichage DBG
 ******************************************************************************/
HAL_StatusTypeDef THW_COM_printf(const char *fmt, ...)
{
	va_list argp;
	int len;

	thw_com_lock();
	va_start(argp, fmt);

	// build string
	if(vsnprintf(thw_comfmt, THW_COM_MAX_FORMAT_LENGTH, fmt, argp) >= 0){
		len = strlen((char*)thw_comfmt);
		HAL_UART_Transmit(&thw_uartHdl, (uint8_t*)thw_comfmt, len, THW_COM_MAX_FORMAT_LENGTH);
	}
	va_end(argp);
	thw_com_unlock();
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		THW_displayActionMenu()
 ** Descriptions:		Affichage du menu d'action
 ******************************************************************************/
void THW_displayActionMenu(st_thw_menuItem *pMenuItems, uint8_t menuItemsQty)
{
	for(uint16_t ind = 0; ind < menuItemsQty; ind++){
		if(pMenuItems[ind].pActionFn != NULL){
			THW_printf("%2d - %s\r\n", ind + 1,	pMenuItems[ind].name);
		}else{
			THW_printf("\r\n");
		}
	}

	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}
#endif //MODE_THW
