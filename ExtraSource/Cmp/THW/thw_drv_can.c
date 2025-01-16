
/*
 * thw_drv_can.c
 *
 *  Created on: 30 jan 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * le CAN\n
 * Le but du jeu étant d'initialiser le can sans avoir le parser d'actif.
 * On fait donc appel aux fonctions suivantes:
 * 		- Pour la reception:
 * 			- FDC_test_rx_init()
 * 			- FDC_test_rx_exit()
 * 		- Pour l'émission:
 * 			- FDC_test_tx_sendMsgToCan()
 *
 *	Les callbacks (HAL_FDCAN_RxFifo0Callback et HAL_FDCAN_RxFifo1Callback) fourniront
 *	les messages reçus dans les mailqueues rx.mq après l'initialisation de celles-ci
 *	par la fonction FDC_test_rx_init().
 *
 *
 *	Sur Can1, on receptionne sur la message Queue Can1_prs (mode Se)
 *	Sur Can2, on receptionne sur la message Queue Can1_tx (mode Bridge)
 *
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <stm32h7xx_hal_fdcan.h>
#include <app_configFdc.h>
#include "fdcan.h"
#include <THW/thw.h>
#include <FDC/fdc_api.h>
#include <FDC/fdc_local.h>
#include <string.h>
#include <task_config.h>
#include <assertError.h>




FDCAN_FilterTypeDef thw_drv_can_fdc1_filterTab[] = {
	{
		// SE Mode Filtering
		.FilterIndex = 	0,
		.IdType = 		FDCAN_STANDARD_ID,
		.FilterType = 	FDCAN_FILTER_RANGE,
		.FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
		.FilterID1 = 	0x000,
		.FilterID2 = 	0x7FF,
	}
};
uint8_t thw_drv_can_fdc1_filterTab_size = sizeof(thw_drv_can_fdc1_filterTab)/sizeof(FDCAN_FilterTypeDef);

FDCAN_FilterTypeDef thw_drv_can_fdc2_filterTab[] = {
	{
		// SE Mode Filtering
		.FilterIndex = 	0,
		.IdType = 		FDCAN_STANDARD_ID,
		.FilterType = 	FDCAN_FILTER_RANGE,
		.FilterConfig = FDCAN_FILTER_TO_RXFIFO1,
		.FilterID1 = 	0x000,
		.FilterID2 = 	0x7FF,
	}
};
uint8_t thw_drv_can_fdc2_filterTab_size = sizeof(thw_drv_can_fdc2_filterTab)/sizeof(FDCAN_FilterTypeDef);











const char thw_drv_can_menuName[] = "Driver | CAN";

//-------------------------

static void thw_drv_can_toggleFrame(void);
static void thw_drv_can_startStopPeriodicMesg_can1(void);
static void thw_drv_can_razQty_can1(void);
static void thw_drv_can_startStopPeriodicMesg_can2(void);
static void thw_drv_can_razQty_can2(void);
static void thw_drv_can_stopAll(void);
static void thw_drv_can_periodInc(void);
static void thw_drv_can_periodDec(void);

st_thw_menuItem thw_drv_can_menuTab[] = {
		{.name = "Toggle CAN Frame", 				.pActionFn = thw_drv_can_toggleFrame, 					.info = 0},
		{.name = "", 								.pActionFn = NULL, 										.info = 0},
		{.name = "CAN1 Sending Frame: Start/Stop", 	.pActionFn = thw_drv_can_startStopPeriodicMesg_can1, 	.info = 0},
		{.name = "CAN1 Raz Quantity", 				.pActionFn = thw_drv_can_razQty_can1, 					.info = 0},
		{.name = "", 								.pActionFn = NULL, 										.info = 0},
		{.name = "CAN2 Sending Frame: Start/Stop", 	.pActionFn = thw_drv_can_startStopPeriodicMesg_can2, 	.info = 0},
		{.name = "CAN2 Raz Quantity", 				.pActionFn = thw_drv_can_razQty_can2, 					.info = 0},
		{.name = "", 								.pActionFn = NULL, 										.info = 0},
		{.name = "CAN Stop All", 					.pActionFn = thw_drv_can_stopAll, 						.info = 0},
		{.name = "CAN Period Inc", 					.pActionFn = thw_drv_can_periodInc, 					.info = 0},
		{.name = "CAN Period Dec", 					.pActionFn = thw_drv_can_periodDec, 					.info = 0},
};
uint16_t thw_drv_can_menuTabSize = sizeof(thw_drv_can_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_can_DisplayMenu(void);
static void thw_drv_can_ManageChoice(char CodeToManage);
static void thw_drv_can_RefreshFn(void);

typedef struct{
	uint16_t	id;
	uint8_t 	data[10];
	uint8_t 	size;
}thw_drv_can_frameItem_t;

static thw_drv_can_frameItem_t thw_drv_can_frameItemList[] = {
		{.id = 0x100, .data = {0xA5, 0xB6, 0xC7, 0xD8, 0xE9}, 	.size = 5},
		{.id = 0x1B0, .data = {0x00, 0x00}, 					.size = 2},
};
static uint8_t thw_drv_can_frameItemList_size = sizeof(thw_drv_can_frameItemList) / sizeof(thw_drv_can_frameItem_t);
static uint8_t thw_drv_can_frameItemList_ind = 0;
static void thw_drv_can_displayFrameSelection(void);

#define THW_DRV_CAN_SENDING_PERIOD 50
uint16_t thw_drv_can_sendingPeriod = THW_DRV_CAN_SENDING_PERIOD;

//-----------------------------------------------------------------------------
// THREAD CAN1
//-----------------------------------------------------------------------------
osThreadId_t thw_drv_can1_tsk_id;
const osThreadAttr_t thw_drv_can1_tsk_attr = {
		.name = "thw_drv_can1_tsk",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static bool thw_drv_can1_tsk_isRunning = false;
static bool thw_drv_can1_tsk_isSending = false;
static void thw_drv_can1_tsk_fn(void *arg);
fdc_rx_mq_item_t thw_drv_can1_msg;
static bool 	thw_drv_can1_newRxMsg = false;
static bool 	thw_drv_can1_newTxMsg = false;
static uint32_t thw_drv_can1_newTxMsg_TS = 0;
fdc_Stats_counters_st		*pThw_drv_can1_stat;
static uint32_t thw_drv_can1_rx_qty = 0;
static uint32_t thw_drv_can1_tx_qty = 0;

//-----------------------------------------------------------------------------
// THREAD CAN2
//-----------------------------------------------------------------------------
osThreadId_t thw_drv_can2_tsk_id;
const osThreadAttr_t thw_drv_can2_tsk_attr = {
		.name = "thw_drv_can2_tsk",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static bool thw_drv_can2_tsk_isRunning = false;
static bool thw_drv_can2_tsk_isSending = false;
static void thw_drv_can2_tsk_fn(void *arg);
fdc_rx_mq_item_t thw_drv_can2_msg;
static bool 	thw_drv_can2_newRxMsg = false;
static bool 	thw_drv_can2_newTxMsg = false;
static uint32_t thw_drv_can2_newTxMsg_TS = 0;
fdc_Stats_counters_st		*pThw_drv_can2_stat;
static uint32_t thw_drv_can2_rx_qty = 0;
static uint32_t thw_drv_can2_tx_qty = 0;


static void thw_drv_can_displayTrame(uint8_t * pData, uint8_t dataLenght);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_can_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_can_setActive(void)
{
	thw_drv_can1_tsk_isRunning = false;
	thw_drv_can1_tsk_isSending = false;
	thw_drv_can2_tsk_isRunning = false;
	thw_drv_can2_tsk_isSending = false;
	thw_drv_can1_rx_qty = 0;
	thw_drv_can1_tx_qty = 0;
	thw_drv_can2_rx_qty = 0;
	thw_drv_can2_tx_qty = 0;
	thw_drv_can1_newRxMsg = false;
	thw_drv_can1_newTxMsg = false;
	thw_drv_can2_newRxMsg = false;
	thw_drv_can2_newTxMsg = false;

	thw_drv_can_sendingPeriod = THW_DRV_CAN_SENDING_PERIOD;


	// Creating Message Queue PRS1
	fdc1_hdl.prs.mq.id = osMessageQueueNew(
			fdc1_hdl.prs.mq.msgQty,
			fdc1_hdl.prs.mq.msgSize,
			&(fdc1_hdl.prs.mq.attr));
	if (fdc1_hdl.prs.mq.id == NULL) {
		return;
	}

	// Creating Message Queue Tx
	fdc1_hdl.tx.mq.id = osMessageQueueNew(
			fdc1_hdl.tx.mq.msgQty,
			fdc1_hdl.tx.mq.msgSize,
			&(fdc1_hdl.tx.mq.attr));
	if (fdc1_hdl.tx.mq.id == NULL) {
		return;
	}

	if(FDC_test_rx_init(&fdc1_hdl, thw_drv_can_fdc1_filterTab, thw_drv_can_fdc1_filterTab_size) != HAL_OK){
		ASSERT_ERROR("FDC_test_rx_init - fdc1_hdl");
	}
	if(FDC_test_rx_init(&fdc2_hdl, thw_drv_can_fdc2_filterTab, thw_drv_can_fdc2_filterTab_size) != HAL_OK){
		ASSERT_ERROR("FDC_test_rx_init - fdc2_hdl");
	}

	// Reset FDCAN stat
	FDC_ResetStat(&fdc1_hdl);
	FDC_ResetStat(&fdc2_hdl);

	pThw_drv_can1_stat = FDC_getStat(&fdc1_hdl);
	pThw_drv_can2_stat = FDC_getStat(&fdc2_hdl);

	// Creating Task 1
	if(	thw_drv_can1_tsk_isRunning == false){
		thw_drv_can1_tsk_id = osThreadNew(thw_drv_can1_tsk_fn, (void *)(&fdc1_hdl), &thw_drv_can1_tsk_attr);
		if(thw_drv_can1_tsk_id == NULL){
			THW_printf("ERROR creating task(%s)\r\n", thw_drv_can1_tsk_attr.name);
		}else{
			thw_drv_can1_tsk_isRunning = true;
		}
	}

	// Creating Task 2
	if(	thw_drv_can2_tsk_isRunning == false){
		thw_drv_can2_tsk_id = osThreadNew(thw_drv_can2_tsk_fn, (void *)(&fdc2_hdl), &thw_drv_can2_tsk_attr);
		if(thw_drv_can2_tsk_id == NULL){
			THW_printf("ERROR creating task(%s)\r\n", thw_drv_can2_tsk_attr.name);
		}else{
			thw_drv_can2_tsk_isRunning = true;
		}
	}


	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_can_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_can_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_can_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	200;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_can_menuName);
	THW_printf("\r\n");

	THW_printf("Frame Selection:\r\n");
	thw_drv_can_displayFrameSelection();
	THW_printf("\r\n");
	THW_printf("CAN1 Rx - Qty  : %d\r\n", thw_drv_can1_rx_qty);
	THW_printf("     Rx - Id   : \r\n");
	THW_printf("     Rx - Data : \r\n");
	THW_printf("     Rx - Time : \r\n");
	THW_printf("     Rx - Error: \r\n");
	THW_printf("\r\n");
	THW_printf("CAN1 Tx - Qty  : %d\r\n", thw_drv_can1_tx_qty);
	THW_printf("     Tx - Id   : \r\n");
	THW_printf("     Tx - Data : \r\n");
	THW_printf("     Tx - Time : \r\n");
	THW_printf("     Tx - Error: \r\n");
	THW_printf("\r\n");
	THW_printf("CAN2 Rx - Qty  : %d\r\n", thw_drv_can2_rx_qty);
	THW_printf("     Rx - Id   : \r\n");
	THW_printf("     Rx - Data : \r\n");
	THW_printf("     Rx - Time : \r\n");
	THW_printf("     Rx - Error: \r\n");
	THW_printf("\r\n");
	THW_printf("CAN2 Tx - Qty  : %d\r\n", thw_drv_can2_tx_qty);
	THW_printf("     Tx - Id   : \r\n");
	THW_printf("     Tx - Data : \r\n");
	THW_printf("     Tx - Time : \r\n");
	THW_printf("     Tx - Error: \r\n");
	THW_printf("\r\n");
	THW_printf("Period Tx: %d ms\r\n", thw_drv_can_sendingPeriod);
	THW_printf("\r\n");

	// Affichage du menu
	THW_displayActionMenu(thw_drv_can_menuTab, thw_drv_can_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_can_menuTabSize)){
		if(thw_drv_can_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_can_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		if(	thw_drv_can1_tsk_isRunning == true){
			osThreadTerminate(thw_drv_can1_tsk_id);
			thw_drv_can1_tsk_isRunning = false;
		}
		if(	thw_drv_can2_tsk_isRunning == true){
			osThreadTerminate(thw_drv_can2_tsk_id);
			thw_drv_can2_tsk_isRunning = false;
		}

		FDC_test_rx_exit(&fdc1_hdl);
		FDC_test_rx_exit(&fdc2_hdl);

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_can_RefreshFn(void)
{
	uint8_t of = thw_drv_can_frameItemList_size + 2; // Offset

	// Display CAN1 statistics
	if(thw_drv_can1_newRxMsg || thw_drv_can1_newTxMsg){
		THW_goto(7 + of,18);	THW_printf("%d"VT100_CLEAREOL, pThw_drv_can1_stat->cpt_err_cb);
		THW_goto(13 + of,18);	THW_printf("%d"VT100_CLEAREOL, pThw_drv_can1_stat->cpt_sendMsgToCanError_cb);
	}

	// Display CAN1 Rx details
	if(thw_drv_can1_newRxMsg){
		THW_goto(3 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can1_rx_qty);
		THW_goto(4 + of,18);	THW_printf("0x%03X"VT100_CLEAREOL, thw_drv_can1_msg.Identifier);
		THW_goto(5 + of,18);	thw_drv_can_displayTrame(thw_drv_can1_msg.RxData, thw_drv_can1_msg.DataLength);
		THW_goto(6 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can1_msg.RxTimestamp);
		thw_drv_can1_newRxMsg = false;
	}

	// Display CAN1 Tx details
	if(thw_drv_can1_newTxMsg){
		THW_goto(9 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can1_tx_qty);
		THW_goto(10 + of,18);	THW_printf("0x%03X\r\n", thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].id);
		THW_goto(11 + of,18);	thw_drv_can_displayTrame(thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].data, thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].size);
		THW_goto(12 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can1_newTxMsg_TS);
		thw_drv_can1_newTxMsg = false;
	}

	// Display CAN2 statistics
	if(thw_drv_can2_newRxMsg || thw_drv_can2_newTxMsg){
		THW_goto(19 + of,18);	THW_printf("%d"VT100_CLEAREOL, pThw_drv_can2_stat->cpt_err_cb);
		THW_goto(25 + of,18);	THW_printf("%d"VT100_CLEAREOL, pThw_drv_can2_stat->cpt_sendMsgToCanError_cb);
	}

	// Display CAN2 Rx details
	if(thw_drv_can2_newRxMsg){
		THW_goto(15 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can2_rx_qty);
		THW_goto(16 + of,18);	THW_printf("0x%03X"VT100_CLEAREOL, thw_drv_can2_msg.Identifier);
		THW_goto(17 + of,18);	thw_drv_can_displayTrame(thw_drv_can2_msg.RxData, thw_drv_can2_msg.DataLength);
		THW_goto(18 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can2_msg.RxTimestamp);
		thw_drv_can2_newRxMsg = false;
	}

	// Display CAN2 Tx details
	if(thw_drv_can2_newTxMsg){
		THW_goto(21 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can2_tx_qty);
		THW_goto(22 + of,18);	THW_printf("0x%03X\r\n", thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].id);
		THW_goto(23 + of,18);	thw_drv_can_displayTrame(thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].data, thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].size);
		THW_goto(24 + of,18);	THW_printf("%d"VT100_CLEAREOL, thw_drv_can2_newTxMsg_TS);
		thw_drv_can2_newTxMsg = false;
	}

}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_toggleFrame(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_toggleFrame(void)
{
	thw_drv_can_frameItemList_ind = (thw_drv_can_frameItemList_ind + 1) % thw_drv_can_frameItemList_size;
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_startStopPeriodicMesg_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_startStopPeriodicMesg_can1(void)
{
	if(	thw_drv_can1_tsk_isSending == false){
		thw_drv_can1_tsk_isSending = true;
	}else{
		thw_drv_can1_tsk_isSending = false;
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_razQty_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_razQty_can1(void)
{
	thw_drv_can1_rx_qty = 0;
	thw_drv_can1_tx_qty = 0;
	FDC_ResetStat(&fdc1_hdl);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_startStopPeriodicMesg_can2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_startStopPeriodicMesg_can2(void)
{
	if(	thw_drv_can2_tsk_isSending == false){
		thw_drv_can2_tsk_isSending = true;
	}else{
		thw_drv_can2_tsk_isSending = false;
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_razQty_can2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_razQty_can2(void)
{
	thw_drv_can2_rx_qty = 0;
	thw_drv_can2_tx_qty = 0;
	FDC_ResetStat(&fdc2_hdl);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_stopAll(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_stopAll(void)
{
	thw_drv_can1_tsk_isSending = false;
	thw_drv_can2_tsk_isSending = false;
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_periodInc(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_periodInc(void)
{
	if(thw_drv_can_sendingPeriod < 5){
		thw_drv_can_sendingPeriod += 1;
	}else if(thw_drv_can_sendingPeriod < 50){
		thw_drv_can_sendingPeriod += 5;
	}else if(thw_drv_can_sendingPeriod < 100){
		thw_drv_can_sendingPeriod += 10;
	}else if(thw_drv_can_sendingPeriod < 1000){
		thw_drv_can_sendingPeriod += 100;
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_periodDec(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_periodDec(void)
{
	if(thw_drv_can_sendingPeriod > 100){
		thw_drv_can_sendingPeriod -= 100;
	}else if(thw_drv_can_sendingPeriod > 50){
		thw_drv_can_sendingPeriod -= 10;
	}else if(thw_drv_can_sendingPeriod > 5){
		thw_drv_can_sendingPeriod -= 5;
	}else if(thw_drv_can_sendingPeriod > 1){
		thw_drv_can_sendingPeriod -= 1;
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_displayFrameSelection(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_displayFrameSelection(void)
{
	for(uint8_t i = 0; i < thw_drv_can_frameItemList_size; i++){
		if(i == thw_drv_can_frameItemList_ind)
			THW_printf(" -->");
		else
			THW_printf("    ");
		THW_printf(" 0x%03X.", thw_drv_can_frameItemList[i].id);
		for(uint8_t j = 0; j < thw_drv_can_frameItemList[i].size; j++){
			THW_printf("%02X ", thw_drv_can_frameItemList[i].data[j]);
		}
		THW_printf(VT100_CLEAREOL"\r\n");
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_can_displayTrame(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_can_displayTrame(uint8_t * pData, uint8_t dataLenght)
{
	if(dataLenght != 0){
		for(uint8_t i = 0; i < dataLenght; i++){
			THW_printf("0x%02X ", pData[i]);
		}
	}
	THW_printf(VT100_CLEAREOL"\r\n");

}

//=============================================================================
//						Thread Function
//=============================================================================
static void thw_drv_can1_tsk_fn(void *argument)
{
	osStatus_t 			status;
	static uint32_t 	lastTick = 0;
	while(1)
	{
		// RX
		status = osMessageQueueGet(fdc1_hdl.prs.mq.id, &thw_drv_can1_msg, NULL, thw_drv_can_sendingPeriod);   // wait for message
		if (status == osOK){
			// Stat
			thw_drv_can1_msg.RxTimestamp = HAL_GetTick();
			thw_drv_can1_rx_qty++;
			thw_drv_can1_newRxMsg = true;
		}

		// TX
		uint32_t actualTick = HAL_GetTick();
		if(actualTick - lastTick >= thw_drv_can_sendingPeriod){
			lastTick = actualTick;
			// Sending
			if(thw_drv_can1_tsk_isSending){
				FDC_test_tx_sendMsgToCan(	&fdc1_hdl,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].id,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].size,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].data);
				thw_drv_can1_newTxMsg_TS = HAL_GetTick();
				thw_drv_can1_tx_qty++;
				thw_drv_can1_newTxMsg = true;
			}
		}


		// The period is done with the osMessageQueueGet timeout
	}
}

static void thw_drv_can2_tsk_fn(void *argument)
{
	osStatus_t 			status;
	static uint32_t 	lastTick = 0;
	fdc_tx_mq_item_t 	can2_msg_tx;

	while(1)
	{
		// RX
		status = osMessageQueueGet(fdc1_hdl.tx.mq.id, &can2_msg_tx, NULL, thw_drv_can_sendingPeriod);   // wait for message
		if (status == osOK){
			// Stat
			thw_drv_can2_msg.RxTimestamp = HAL_GetTick();
			thw_drv_can2_msg.DataLength = can2_msg_tx.DataLength;
			thw_drv_can2_msg.Identifier = can2_msg_tx.Identifier;
			memcpy(thw_drv_can2_msg.RxData, can2_msg_tx.TxData, 8 );
			thw_drv_can2_rx_qty++;
			thw_drv_can2_newRxMsg = true;
		}

		// TX
		uint32_t actualTick = HAL_GetTick();
		if(actualTick - lastTick >= thw_drv_can_sendingPeriod){
			lastTick = actualTick;
			// Sending
			if(thw_drv_can2_tsk_isSending){
				FDC_test_tx_sendMsgToCan(	&fdc2_hdl,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].id,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].size,
											thw_drv_can_frameItemList[thw_drv_can_frameItemList_ind].data);
				thw_drv_can2_newTxMsg_TS = HAL_GetTick();
				thw_drv_can2_tx_qty++;
				thw_drv_can2_newTxMsg = true;
			}
		}


		// The period is done with the osMessageQueueGet timeout
	}
}









#endif //MODE_THW
