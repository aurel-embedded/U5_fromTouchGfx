/*
 * dbg_debugPrintf.c
 *
 *  Created on: 8 mars 2018
 */

#include <cmsis_os2.h>
#include <dbg_config.h>
#include <DBG/dbg_debugPrintf.h>
#include "APP.h"

#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define LOG_TIMEOUT_UART 1
#define DBG_TIMEOUT_QUEUE 0
#define DBG_TIMEOUT_UART 1


#define DBG_MAX_FORMAT_LENGTH 256
char dbgfmt[DBG_MAX_FORMAT_LENGTH];

// Mutex
osMutexId_t dbg_mutex_id;
const osMutexAttr_t dbg_mutex_attr = {
		"dbgComMtx",     						// human readable mutex name
		osMutexRecursive | osMutexPrioInherit,  // attr_bits
		NULL,                					// memory for control block
		0U                   					// size for control block
};
#define DBG_MTX_WAIT_TIME	500

static bool dbg_cmpEnable = false;

UART_HandleTypeDef dbg_uart_hdl;


/******************************************************************************
 ** Function name:		DBG_init
 ** Descriptions:		initialise le module DBG
 ** parameters:			NA
 ** Returned value:		Status
 ******************************************************************************/
HAL_StatusTypeDef DBG_init(void)
{
	dbg_uart_hdl.Instance            = DBG_UART;
	dbg_uart_hdl.Init.BaudRate       = (115200 * 4);
	dbg_uart_hdl.Init.WordLength     = UART_WORDLENGTH_8B;
	dbg_uart_hdl.Init.StopBits       = UART_STOPBITS_1;
	dbg_uart_hdl.Init.Parity         = UART_PARITY_NONE;
	dbg_uart_hdl.Init.Mode           = UART_MODE_TX_RX;
	dbg_uart_hdl.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
	dbg_uart_hdl.Init.OverSampling   = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&dbg_uart_hdl) != HAL_OK){
		return HAL_ERROR;
	}

	// Create Mutex
	dbg_mutex_id = osMutexNew(&dbg_mutex_attr);
	if (dbg_mutex_id == NULL)  {
		// Mutex object not created
		return HAL_ERROR;
	}

	dbg_cmpEnable = true;

	return HAL_OK;
}


/******************************************************************************
 ** Function name:		DBG_exit
 ** Descriptions:		finalise le module DBG
 ** parameters:			None
 ** Returned value:		Status
 ******************************************************************************/
HAL_StatusTypeDef DBG_exit(void)
{
	// Suppression mutex
	osMutexDelete(dbg_mutex_id);

	return HAL_OK;
}

/******************************************************************************
 ** Function name:		DBG_lock()
 ** Descriptions:		Lock le mutex de l'affichage DBG
 ******************************************************************************/
void DBG_lock(void)
{
	if(dbg_cmpEnable == false)
		return;

	if(osKernelGetState() == osKernelRunning)
		osMutexAcquire(dbg_mutex_id, osWaitForever);
}
/******************************************************************************
 ** Function name:		DBG_unlock()
 ** Descriptions:		Unloock le mutex de l'affichage DBG
 ******************************************************************************/
void DBG_unlock(void)
{
	if(dbg_cmpEnable == false)
		return;
	if(osKernelGetState() == osKernelRunning)
		osMutexRelease(dbg_mutex_id);
}

/******************************************************************************
 ** Function name:		DBG_printfNoLock()
 ** Descriptions:		Envoi asysnchrone sur la console d'affichage DBG
 ******************************************************************************/
HAL_StatusTypeDef DBG_printfNoLock(const char *fmt, ...)
{
	if(dbg_cmpEnable == false)
		return HAL_ERROR;

	va_list argp; int len;
	va_start(argp, fmt);

	// build string
	if(vsnprintf(dbgfmt, DBG_MAX_FORMAT_LENGTH, fmt, argp) >= 0){
		len = strlen((char*)dbgfmt);
		HAL_UART_Transmit(&dbg_uart_hdl, (uint8_t*)dbgfmt, len, DBG_MAX_FORMAT_LENGTH * DBG_TIMEOUT_UART);
	}
	va_end(argp);
	return HAL_OK;
}

/******************************************************************************
 ** Function name:		DBG_printf()
 ** Descriptions:		Envoi asysnchrone sur la console d'affichage DBG
 ******************************************************************************/
HAL_StatusTypeDef DBG_printf(const char *fmt, ...)
{
	if(dbg_cmpEnable == false)
		return HAL_ERROR;

	va_list argp;
	int len;

	DBG_lock();
	va_start(argp, fmt);

	// build string
	if(vsnprintf(dbgfmt, DBG_MAX_FORMAT_LENGTH, fmt, argp) >= 0){
		len = strlen((char*)dbgfmt);
		HAL_UART_Transmit(&dbg_uart_hdl, (uint8_t*)dbgfmt, len, DBG_MAX_FORMAT_LENGTH * DBG_TIMEOUT_UART);
	}
	va_end(argp);
	DBG_unlock();
	return HAL_OK;
}


/******************************************************************************
 ** Function name:		DBG_printfLine()
 ** Descriptions:		Envoi asysnchrone sur la console d'affichage DBG
 ******************************************************************************/
HAL_StatusTypeDef DBG_printfLine(uint16_t X, uint16_t Y,const char *fmt, ...)
{
	if(dbg_cmpEnable == false)
		return HAL_ERROR;

	va_list argp; int len;

	DBG_lock();
	DBG_goto(X,Y);
	va_start(argp, fmt);

	// build string
	if(vsnprintf(dbgfmt, DBG_MAX_FORMAT_LENGTH, fmt, argp) >= 0){
		len = strlen((char*)dbgfmt);
		HAL_UART_Transmit(&dbg_uart_hdl, (uint8_t*)dbgfmt, len, DBG_MAX_FORMAT_LENGTH * DBG_TIMEOUT_UART);
	}
	va_end(argp);
	HAL_UART_Transmit(&dbg_uart_hdl, (uint8_t*)VT100_CLEAREOL, strlen(VT100_CLEAREOL), DBG_TIMEOUT_UART);
	HAL_UART_Transmit(&dbg_uart_hdl, (uint8_t*)"\r\n", 2, DBG_TIMEOUT_UART);
	DBG_unlock();

	return HAL_OK;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//								BANNER DISPLAY
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/******************************************************************************
 ** Function name:		dbg_displayBanner()
 ** Descriptions:		Display the main Banner (Bruce!!)
 ******************************************************************************/
#ifdef DEBUG
static const uint8_t 	APP_BuildingInformation1[]	= "Debug";
#else
static const uint8_t 	APP_BuildingInformation1[]	= "Release";
#endif

#ifdef NOBOOTLOADER
static const uint8_t 	APP_BuildingInformation2[]	= "NoBootloader";
#else
static const uint8_t 	APP_BuildingInformation2[]	= "Bootloader";
#endif

void DBG_displayBanner(void)
{
	DBG_clearScreen();
	DBG_printf(VT100_NORMAL);
	DBG_printf("------------------------------------------------------------------\r\n");
	DBG_printf(" %s (v%s - %s - %s) is greeting you!!\r\n", APP_SoftwareReference, APP_SoftwareVersion, APP_BuildingInformation1, APP_BuildingInformation2);
	DBG_printf("------------------------------------------------------------------\r\n");
}


