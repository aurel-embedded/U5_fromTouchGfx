#include <THW_core/THW_testHardware_API.h>

/*
 * THW_hardwareTest.c
 *
 *  Created on: 7 nov. 2022
 *      Author: apajadon
 */
#ifdef MODE_THW

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW/thw.h>
#include <THW_core/THW_testHardware_common.h>
#include "task_config.h"

//-----------------------------------------------------------------------------
// THREAD RX
//-----------------------------------------------------------------------------
osThreadId_t thw_tsk_id;
const osThreadAttr_t thw_tsk_attr = {
		.name = "thw_tsk",
		.stack_size = TSK_CFG__STACK__TSK_THW,
		.priority = TSK_CFG__PRIO__TSK_THW,
};
static void thw_tsk_fn(void *arg);

//-----------------------------------------------------------------------------
// THREAD Refresh
//-----------------------------------------------------------------------------
osThreadId_t thw_rfs_tsk_id;
const osThreadAttr_t thw_rfs_tsk_attr = {
		.name = "thw_rfs_tsk",
		.stack_size = TSK_CFG__STACK__TSK_THW_RFS,
		.priority = TSK_CFG__PRIO__TSK_THW_RFS,
};
static void thw_rfs_tsk_fn(void *arg);
bool 		thw_refreshIsRunning = false;
static HAL_StatusTypeDef thw_startRefreshTask(void);
static HAL_StatusTypeDef thw_stopRefreshTask(void);

//------------------------------------------------------------------------------
// MENU
//------------------------------------------------------------------------------
st_thw_actualMenu thw_actualMenu;	// Structure sur le menu actuellement utilisé

//=============================================================================
//						Thread Functions
//=============================================================================
static void thw_tsk_fn(void *argument)
{
	uint32_t 	userChoice;

	// Display Main Menu
	thw_main_setActive();

	THW_clearScreen();
	if(thw_actualMenu.displayMenu != NULL)
		thw_actualMenu.displayMenu();

	while(1){
		osDelay(100);

		// Get User Choice
		//----------------------
		if(thw_com_manageRx() == true){

	    	// 	Manage user Choice
			//----------------------
			userChoice = atoi((char*)thw_line);
			if(thw_actualMenu.manageChoiceFn != NULL)
				thw_actualMenu.manageChoiceFn(userChoice);

			// 		Display
			//----------------------
			if(thw_actualMenu.clearScreen == true){
				THW_clearScreen();
			}else{
				THW_goto(0,0);
				thw_actualMenu.clearScreen = true;	// No Clear only one time
			}
			if(thw_actualMenu.displayMenu != NULL)
				thw_actualMenu.displayMenu();

			// 		Refresh
			//----------------------
			if(thw_actualMenu.refreshFn != NULL)
				thw_startRefreshTask();
			else
				thw_stopRefreshTask();
		}
	}
}


//------------------------------------------------------------------------------
/// 								Refresh Thread
//------------------------------------------------------------------------------
static void thw_rfs_tsk_fn(void *arg)
{
	thw_refreshIsRunning = true;
	while(thw_refreshIsRunning == true)
	{
		// Si mode refresh actif
		if(thw_actualMenu.refreshFn != NULL){
			THW_saveCurPos();				// Sauvegarde position curseur
			thw_actualMenu.refreshFn();		// Execution fonction Refresh
			THW_restoreCurPos();			// Restoration position curseur
		}

		// Période
		if(thw_actualMenu.refreshPeriodInMs < 50)
			osDelay(50);
		else if (thw_actualMenu.refreshPeriodInMs > 2000)
			osDelay(2000);
		else
			osDelay(thw_actualMenu.refreshPeriodInMs);
	}
	osThreadTerminate(thw_rfs_tsk_id);
}


//=============================================================================
//						Externals Functions (API)
//=============================================================================
/******************************************************************************
 ** Function name:		THW_init
 ** Descriptions:		THW component Init
 ******************************************************************************/
HAL_StatusTypeDef THW_init(void)
{

	// Low Level Init
	if(thw_com_init() != HAL_OK){
		return HAL_ERROR;
	}

	// Creating Task
	thw_tsk_id = osThreadNew(thw_tsk_fn, NULL, &thw_tsk_attr);
	if(thw_tsk_id == NULL){
		return HAL_ERROR;
	}

	return HAL_OK;

}



/******************************************************************************
 ** Function name:		THW_avoidClearScreen
 ** Descriptions:		Avoid to clear the screen
 ******************************************************************************/
void THW_avoidClearScreen(void)
{
	thw_actualMenu.clearScreen = false;
}


/******************************************************************************
 ** Function name:		thw_startRefreshTask
 ** Descriptions:		Démarre la tâche Refresh
 ** parameters:			NA
 ** Returned value:		Status
 ******************************************************************************/
static HAL_StatusTypeDef thw_startRefreshTask(void)
{
	if(thw_refreshIsRunning != true){
		// Création de la tâche Refresh
		//----------------------------------
		thw_rfs_tsk_id = osThreadNew(thw_rfs_tsk_fn, NULL, &thw_rfs_tsk_attr);
		if(thw_rfs_tsk_id == NULL){
			return(HAL_ERROR);
		}
	}

	return(HAL_OK);
}

/******************************************************************************
 ** Function name:		thw_stopRefreshTask
 ** Descriptions:		Arrêt de la tâche Refresh
 ** parameters:			NA
 ** Returned value:		Status
 ******************************************************************************/
static HAL_StatusTypeDef thw_stopRefreshTask(void)
{
	thw_refreshIsRunning = false;
	return HAL_OK;
}


#endif //MODE_THW
