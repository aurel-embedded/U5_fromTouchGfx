/*
 * thw_menuCmpFdc.c
 *
 *  Created on: 26 jan 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Fdcan\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <cmsis_os2.h>
#include "APP.h"
#include "FDC/fdc_api.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include "fdc_config.h"
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>


const char thw_cmp_fdc_norm_menuName[] = "Component | FDC | Normal";

static void thw_cmp_fdc_norm_sendMesg_can1(void);
static void thw_cmp_fdc_norm_sendMesg_can2(void);
static void thw_cmp_fdc_norm_sendPeriodicMesg_can1(void);
static void thw_cmp_fdc_norm_stopPeriodicMesg_can1(void);
static void thw_cmp_fdc_norm_sendPeriodicMesg_can2(void);
static void thw_cmp_fdc_norm_stopPeriodicMesg_can2(void);

st_thw_menuItem thw_cmp_fdc_norm_menuTab[] = {
		{.name = "Send CAN1 Set Motor Speed", 	.pActionFn = thw_cmp_fdc_norm_sendMesg_can1, 			.info = 0},
		{.name = "Send CAN2 Set Motor Speed", 	.pActionFn = thw_cmp_fdc_norm_sendMesg_can2, 			.info = 0},
		{.name = "Send CAN1 Frame frequently", 	.pActionFn = thw_cmp_fdc_norm_sendPeriodicMesg_can1, 	.info = 0},
		{.name = "Stop CAN1 Frame Send", 		.pActionFn = thw_cmp_fdc_norm_stopPeriodicMesg_can1, 	.info = 0},
		{.name = "Send CAN2 Frame frequently", 	.pActionFn = thw_cmp_fdc_norm_sendPeriodicMesg_can2, 	.info = 0},
		{.name = "Stop CAN2 Frame Send", 		.pActionFn = thw_cmp_fdc_norm_stopPeriodicMesg_can2, 	.info = 0},
};
uint16_t thw_cmp_fdc_norm_menuTabSize = sizeof(thw_cmp_fdc_norm_menuTab) / sizeof(st_thw_menuItem);


static void thw_cmp_fdc_norm_DisplayMenu(void);
static void thw_cmp_fdc_norm_ManageChoice(char CodeToManage);
static void thw_cmp_fdc_norm_RefreshFn(void);

static bool thw_cmp_fdc_norm_tsk1_isRunning = false;
static bool thw_cmp_fdc_norm_tsk2_isRunning = false;

static uint8_t thw_cmp_fdc_norm_cpt = 0;

//-----------------------------------------------------------------------------
// THREAD
//-----------------------------------------------------------------------------
osThreadId_t thw_fdc_sendMsg_tsk1_id;
const osThreadAttr_t thw_fdc_sendMsg_tsk1_attr = {
		.name = "thw_fdc_sendMsg_tsk1",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
osThreadId_t thw_fdc_sendMsg_tsk2_id;
const osThreadAttr_t thw_fdc_sendMsg_tsk2_attr = {
		.name = "thw_fdc_sendMsg_tsk2",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static void thw_fdc_sendMsg_fn(void *arg);


//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_fdc_norm_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_fdc_norm_setActive(void)
{
	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;
	if(FDC_init(&fdc2_hdl, fdc2_filterTab, fdc2_filterTab_size) != HAL_OK)
		return;

	thw_cmp_fdc_norm_tsk1_isRunning = false;
	thw_cmp_fdc_norm_tsk2_isRunning = false;
	thw_cmp_fdc_norm_cpt = 0;

	// Menu affiché
	thw_actualMenu.displayMenu = 	thw_cmp_fdc_norm_DisplayMenu;
	thw_actualMenu.manageChoiceFn = thw_cmp_fdc_norm_ManageChoice;
	thw_actualMenu.refreshFn = 		thw_cmp_fdc_norm_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 250;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_fdc_norm_menuName);
	THW_printf("\r\n");

	THW_goto(3,28); THW_printf("CAN1");
	THW_goto(3,48); THW_printf("CAN2\r\n");
	THW_printf("cpt_itRx :\r\n");
	THW_printf("cpt_taskRx_threadFlag :\r\n");
	THW_printf("cpt_taskRx_ParserMsg_sendOk :\r\n");
	THW_printf("cpt_taskRx_ParserMsg_sendKo :\r\n");
	THW_printf("cpt_taskRx_BridgeMsg_sendOk :\r\n");
	THW_printf("cpt_taskRx_BridgeMsg_sendKo :\r\n");
	THW_printf("cpt_err_cb :\r\n");
	THW_printf("cpt_txFifoEmpty_cb :\r\n");
	THW_printf("cpt_sendMsgToCanOK_cb :\r\n");
	THW_printf("cpt_sendMsgToCanError_cb :\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_fdc_norm_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_fdc_norm_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_fdc_norm_menuTabSize)){
		if(thw_cmp_fdc_norm_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_fdc_norm_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){

		// Deleting Task
		thw_cmp_fdc_norm_stopPeriodicMesg_can1();
		thw_cmp_fdc_norm_stopPeriodicMesg_can2();

		// Finalize Component
		FDC_exit(&fdc1_hdl);
		FDC_exit(&fdc2_hdl);

		// Return to the previous menu
		thw_cmp_fdc_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_cmp_fdc_norm_RefreshFn(void)
{
	fdc_Stats_counters_st *pStat1 = FDC_getStat(&fdc1_hdl);
	fdc_Stats_counters_st *pStat2 = FDC_getStat(&fdc2_hdl);

	THW_goto(4,30);  THW_printf("%ld", pStat1->cpt_itRx); 							THW_goto(4,50);  THW_printf( "%ld", pStat2->cpt_itRx);
	THW_goto(5,30);  THW_printf("%ld", pStat1->cpt_taskRx_threadFlag); 				THW_goto(5,50);  THW_printf( "%ld", pStat2->cpt_taskRx_threadFlag);
	THW_goto(6,30);  THW_printf("%ld", pStat1->cpt_taskRx_ParserMsg_sendOk); 		THW_goto(6,50);  THW_printf( "%ld", pStat2->cpt_taskRx_ParserMsg_sendOk);
	THW_goto(7,30);  THW_printf("%ld", pStat1->cpt_taskRx_ParserMsg_sendKo); 		THW_goto(7,50);  THW_printf( "%ld", pStat2->cpt_taskRx_ParserMsg_sendKo);
	THW_goto(8,30);  THW_printf("%ld", pStat1->cpt_taskRx_BridgeMsg_sendOk); 		THW_goto(8,50);  THW_printf( "%ld", pStat2->cpt_taskRx_BridgeMsg_sendOk);
	THW_goto(9,30);  THW_printf("%ld", pStat1->cpt_taskRx_BridgeMsg_sendKo); 		THW_goto(9,50);  THW_printf( "%ld", pStat2->cpt_taskRx_BridgeMsg_sendKo);
	THW_goto(10,30); THW_printf("%ld", pStat1->cpt_err_cb); 						THW_goto(10,50); THW_printf( "%ld", pStat2->cpt_err_cb);
	THW_goto(11,30); THW_printf("%ld", pStat1->cpt_txFifoEmpty_cb); 				THW_goto(11,50); THW_printf( "%ld", pStat2->cpt_txFifoEmpty_cb);
	THW_goto(12,30); THW_printf("%ld", pStat1->cpt_sendMsgToCanOK_cb); 				THW_goto(12,50); THW_printf( "%ld", pStat2->cpt_sendMsgToCanOK_cb);
	THW_goto(13,30); THW_printf("%ld", pStat1->cpt_sendMsgToCanError_cb); 			THW_goto(13,50); THW_printf( "%ld", pStat2->cpt_sendMsgToCanError_cb);

}




//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//=============================================================================
//						Thread Function
//=============================================================================
static void thw_fdc_sendMsg_fn(void *argument)
{
	fdc_hdl_t * pCan_hdl = (fdc_hdl_t*)argument;

	uint8_t txData[8] = {0x0E, 0xF2, 0xFE, 0x03, 0xE8};

	while(1){

		FDC_sendMsg(pCan_hdl, 0x1A8 ,5, txData);
		osDelay(100);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_sendMesg_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_sendMesg_can1(void)
{
	uint8_t txData[8] = {0x0E, 0xF2, 0xFE, 0x03, 0xE8};

	FDC_sendMsg(&fdc1_hdl, 0x1A8 ,5, txData);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_sendMesg_can2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_sendMesg_can2(void)
{
	uint8_t txData[8] = {0x0E, 0xF2, 0xFE, 0x03, 0xE8};

	FDC_sendMsg(&fdc2_hdl, 0x1A8 ,5, txData);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_sendPeriodicMesg_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_sendPeriodicMesg_can1(void)
{
	if(	thw_cmp_fdc_norm_tsk1_isRunning == false){
		// Creating Task
		thw_fdc_sendMsg_tsk1_id = osThreadNew(thw_fdc_sendMsg_fn, (void *)(&fdc1_hdl), &thw_fdc_sendMsg_tsk1_attr);
		if(thw_fdc_sendMsg_tsk1_id == NULL){
			THW_printf("ERROR creating task(%s)\r\n", thw_fdc_sendMsg_tsk1_attr.name);
		}else{
			thw_cmp_fdc_norm_tsk1_isRunning = true;
		}
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_stopPeriodicMesg_can1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_stopPeriodicMesg_can1(void)
{
	// Deleting Task
	if(	thw_cmp_fdc_norm_tsk1_isRunning == true){
		osThreadTerminate(thw_fdc_sendMsg_tsk1_id);
		thw_cmp_fdc_norm_tsk1_isRunning = false;
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_sendPeriodicMesg_can2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_sendPeriodicMesg_can2(void)
{
	if(	thw_cmp_fdc_norm_tsk2_isRunning == false){
		// Creating Task
		thw_fdc_sendMsg_tsk2_id = osThreadNew(thw_fdc_sendMsg_fn, (void *)(&fdc2_hdl), &thw_fdc_sendMsg_tsk2_attr);
		if(thw_fdc_sendMsg_tsk2_id == NULL){
			THW_printf("ERROR creating task(%s)\r\n", thw_fdc_sendMsg_tsk2_attr.name);
		}else{
			thw_cmp_fdc_norm_tsk2_isRunning = true;
		}
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_fdc_norm_stopPeriodicMesg_can2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_fdc_norm_stopPeriodicMesg_can2(void)
{
	if(	thw_cmp_fdc_norm_tsk2_isRunning == true){
		// Deleting Task
		osThreadTerminate(thw_fdc_sendMsg_tsk2_id);
		thw_cmp_fdc_norm_tsk2_isRunning = false;
	}
}

#endif //MODE_THW
