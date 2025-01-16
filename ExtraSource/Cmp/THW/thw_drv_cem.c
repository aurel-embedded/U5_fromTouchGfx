
/*
 * thw_drv_cem.c
 *
 *  Created on: 21 fev 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * les ADC\n
 *
 */
#ifdef MODE_THW

#include <string.h>
#include <cmsis_os2.h>
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include <REG/EVP/evp_api.h>
#include <FAN/drvFan/drvFan.h>
#include <REG/EVS/evs_api.h>
#include <app_configFdc.h>
#include "fdcan.h"
#include <drvAdc/drvAdc_api.h>
#include <FDC/fdc_api.h>



const char thw_drv_cem_menuName[] = "Driver | CEM";


//-----------------------------------------------------------------------------
//									ADC
//-----------------------------------------------------------------------------
static bool thw_drv_cem_adcConvIsRunning = false;

//-----------------------------------------------------------------------------
//									EVP
//-----------------------------------------------------------------------------
uint16_t	thw_drv_cem_evp_commonFreqInKhz = 100;
typedef struct{
	evp_id_t		evpId;					// Evp ID
	uint8_t			requestDutyCycle;		// Duty Cycle in %
}thw_drv_cem_evp_item_t;
thw_drv_cem_evp_item_t thw_drv_cem_evp1 = {.evpId = evp_id1, .requestDutyCycle = 80};
thw_drv_cem_evp_item_t thw_drv_cem_evp2 = {.evpId = evp_id2, .requestDutyCycle = 80};
thw_drv_cem_evp_item_t thw_drv_cem_evp3 = {.evpId = evp_id3, .requestDutyCycle = 80};
static void thw_drv_cem_evp_startStop(thw_drv_cem_evp_item_t *pEvp);

//-----------------------------------------------------------------------------
//									EVS
//-----------------------------------------------------------------------------
uint16_t	thw_drv_cem_evs_commonFreqInKhz = 40;
typedef struct{
	evs_id_t		evsId;			// Evs ID
	uint8_t			dutyCycle;		// Duty Cycle in %
}thw_drv_cem_evs_item_t;

thw_drv_cem_evs_item_t thw_drv_cem_evs1 = {.evsId = evs_id1, .dutyCycle = 80};
thw_drv_cem_evs_item_t thw_drv_cem_evs2 = {.evsId = evs_id2, .dutyCycle = 80};
static void thw_drv_cem_evs_pwmStartStop(thw_drv_cem_evs_item_t *pEvs);

//-----------------------------------------------------------------------------
//									CAN
//-----------------------------------------------------------------------------
typedef struct{
	uint16_t	id;
	uint8_t 	data[10];
	uint8_t 	size;
}thw_drv_cam_can_frameItem_t;
static thw_drv_cam_can_frameItem_t thw_drv_cem_can_frameItem = {
		.id = 	0x1A0,
		.data = {0x00, 0x00},
		.size = 2,
};

#define THW_DRV_CEM_CAN_SENDING_PERIOD 50
osThreadId_t thw_drv_cem_can_tsk_id;
const osThreadAttr_t thw_drv_cem_can_tsk_attr = {
		.name = "thw_drv_cem_can_tsk",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static bool thw_drv_cem_can_tsk_isRunning = false;
static bool thw_drv_cem_can_tsk_isSending = false;
static void thw_drv_cem_can_tsk_fn(void *arg);
fdc_rx_mq_item_t thw_drv_cem_can_msg;
static bool 	thw_drv_cem_can_newRxMsg = false;
static bool 	thw_drv_cem_can_newTxMsg = false;
static uint32_t thw_drv_cem_can_newTxMsg_TS = 0;
fdc_Stats_counters_st *pThw_drv_cem_can_stat;
static uint32_t thw_drv_cem_can_rx_qty = 0;
static uint32_t thw_drv_cem_can_tx_qty = 0;
static bool 	thw_drv_cem_can_isComActiv = false;



//-----------------------------------------------------------------------------
//									PUMP
//-----------------------------------------------------------------------------
bool thw_drv_cem_pump_isActiv = false;
static thw_drv_cam_can_frameItem_t thw_drv_cem_can_pump_go = {
		.id = 	0x293,
		.data = {0x01, 0x01},
		.size = 2,
};
static thw_drv_cam_can_frameItem_t thw_drv_cem_can_pump_stop = {
		.id = 	0x293,
		.data = {0x00, 0x00},
		.size = 2,
};

//-----------------------------------------------------------------------------
//									FAN
//-----------------------------------------------------------------------------
bool thw_drv_cem_fan_pwm_isRunning = false;
static uint16_t 	thw_drv_cem_fan_pwmFreq_value = 25;	// Frequency in kHz
static uint16_t 	thw_drv_cem_fan_pwmDc_value = 	95;	// Duty Cycle in %

//-----------------------------------------------------------------------------


//-------------------------
static void thw_drv_cem_adc_startStop(void);
static void thw_drv_cem_evp1_startStop(void){thw_drv_cem_evp_startStop(&thw_drv_cem_evp1);};
static void thw_drv_cem_evp2_startStop(void){thw_drv_cem_evp_startStop(&thw_drv_cem_evp2);};
static void thw_drv_cem_evp3_startStop(void){thw_drv_cem_evp_startStop(&thw_drv_cem_evp3);};
static void thw_drv_cem_evs1_pwmStartStop(void){thw_drv_cem_evs_pwmStartStop(&thw_drv_cem_evs1);};
static void thw_drv_cem_evs2_pwmStartStop(void){thw_drv_cem_evs_pwmStartStop(&thw_drv_cem_evs2);};
static void thw_drv_cem_evAll_StartStop(void);
static void thw_drv_cem_pump_StartStop(void);
static void thw_drv_cem_fan_pwmStartStop(void);
static void thw_drv_cem_can_startStop(void);

st_thw_menuItem thw_drv_cem_menuTab[] = {
		{.name = "ADC      : start/stop",		.pActionFn = thw_drv_cem_adc_startStop, 	.info = 0},
		{.name = "EVP1     : start/stop",		.pActionFn = thw_drv_cem_evp1_startStop, 	.info = 0},
		{.name = "EVP2     : start/stop",		.pActionFn = thw_drv_cem_evp2_startStop, 	.info = 0},
		{.name = "EVP3     : start/stop",		.pActionFn = thw_drv_cem_evp3_startStop, 	.info = 0},
		{.name = "EVS1     : start/stop",		.pActionFn = thw_drv_cem_evs1_pwmStartStop, .info = 0},
		{.name = "EVS2     : start/stop",		.pActionFn = thw_drv_cem_evs2_pwmStartStop, .info = 0},
		{.name = "EV ALL   : start/stop",		.pActionFn = thw_drv_cem_evAll_StartStop, 	.info = 0},
		{.name = "PUMP     : start/stop",		.pActionFn = thw_drv_cem_pump_StartStop, 	.info = 0},
		{.name = "FAN      : start/stop",		.pActionFn = thw_drv_cem_fan_pwmStartStop, 	.info = 0},
		{.name = "CAN      : start/stop",		.pActionFn = thw_drv_cem_can_startStop, 	.info = 0},


};
uint16_t thw_drv_cem_menuTabSize = sizeof(thw_drv_cem_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_cem_DisplayMenu(void);
static void thw_drv_cem_ManageChoice(char CodeToManage);
static void thw_drv_cem_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_cem_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_cem_setActive(void)
{

	// ADC
	thw_drv_cem_adcConvIsRunning = false;
	DRVADC_init();

	// EVP
	EVP_init();
	EVP_setFrequencyInKHz(thw_drv_cem_evp_commonFreqInKhz);
	EVP_setEvpDutyCycle(evp_id1, thw_drv_cem_evp1.requestDutyCycle);
	EVP_setEvpDutyCycle(evp_id2, thw_drv_cem_evp2.requestDutyCycle);
	EVP_setEvpDutyCycle(evp_id3, thw_drv_cem_evp3.requestDutyCycle);

	// EVS
	EVS_init();
	EVS_setFrequencyInKHz(thw_drv_cem_evs_commonFreqInKhz);
	EVS_setEvsDutyCycle(evs_id1, thw_drv_cem_evs1.dutyCycle);
	EVS_setEvsDutyCycle(evs_id2, thw_drv_cem_evs2.dutyCycle);

	if(FDC_test_rx_init(&fdc1_hdl, NULL, 0) != HAL_OK){
		THW_printf("ERROR creating FDC_test_rx_init(&fdc1_hdl)\r\n");
	}
	if(FDC_test_rx_init(&fdc2_hdl, NULL, 0) != HAL_OK){
		THW_printf("ERROR creating FDC_test_rx_init(&fdc2_hdl)\r\n");
	}

	// PUMP
	thw_drv_cem_pump_isActiv = false;

	// Reset FDCAN stat
	FDC_ResetStat(&fdc1_hdl);
	FDC_ResetStat(&fdc2_hdl);
	thw_drv_cem_can_rx_qty = 0;

	pThw_drv_cem_can_stat = FDC_getStat(&fdc1_hdl);

	// Creating Task
	if(	thw_drv_cem_can_tsk_isRunning == false){
		thw_drv_cem_can_tsk_id = osThreadNew(thw_drv_cem_can_tsk_fn, (void *)(&fdc1_hdl), &thw_drv_cem_can_tsk_attr);
		if(thw_drv_cem_can_tsk_id == NULL){
			THW_printf("ERROR creating task(%s)\r\n", thw_drv_cem_can_tsk_attr.name);
		}else{
			thw_drv_cem_can_tsk_isRunning = true;
		}
	}

	// FAN
	DRVFAN_init();
	DRVFAN_setFrequencyInKHz(thw_drv_cem_fan_pwmFreq_value);
	DRVFAN_setDutyCycle(thw_drv_cem_fan_pwmDc_value);


	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_cem_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_cem_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_cem_RefreshFn;
}

//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_cem_menuName);
	THW_printf("\r\n");

	THW_printf("ADC State          : "); (thw_drv_cem_adcConvIsRunning)? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("\r\n");
	THW_printf("EVP1 (%d kHz %d%%) : ", EVP_getFrequencyInKHz(), EVP_getEvpDutyCycle(evp_id1));(EVP_isEvpRunning(evp_id1))? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("EVP2 (%d kHz %d%%) : ", EVP_getFrequencyInKHz(), EVP_getEvpDutyCycle(evp_id2));(EVP_isEvpRunning(evp_id2))? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("EVP3 (%d kHz %d%%) : ", EVP_getFrequencyInKHz(), EVP_getEvpDutyCycle(evp_id3));(EVP_isEvpRunning(evp_id3))? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("\r\n");
	THW_printf("EVS1 (%d kHz %d%%)  : ", EVS_getFrequencyInKHz(), EVS_getEvsDutyCycle(evs_id1));(EVS_isEvsRunning(evs_id1))? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("EVS2 (%d kHz %d%%)  : ", EVS_getFrequencyInKHz(), EVS_getEvsDutyCycle(evs_id2));(EVS_isEvsRunning(evs_id2))? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("\r\n");
	THW_printf("PUMP               : "); (thw_drv_cem_pump_isActiv)? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("\r\n");
	THW_printf("FAN                : "); (thw_drv_cem_fan_pwm_isRunning)? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("\r\n");
	THW_printf("CAN                : "); (thw_drv_cem_can_tsk_isSending)? THW_printfCL(VT100_BG_GREEN"Run"): THW_printfCL(VT100_BG_RED"Stop");
	THW_printf("Status with T80i   :\r\n");
	THW_printf("CAN Rx             :\r\n");
	THW_printf("CAN Error          :\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_cem_menuTab, thw_drv_cem_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_cem_menuTabSize)){
		if(thw_drv_cem_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_cem_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		thw_drv_cem_adcConvIsRunning = false;
		FDC_test_tx_sendMsgToCan(	&fdc1_hdl,
									thw_drv_cem_can_pump_stop.id,
									thw_drv_cem_can_pump_stop.size,
									thw_drv_cem_can_pump_stop.data);

		DRVADC_stopAdc();
		DRVADC_exit();
		EVS_exit();
		EVP_exit();
		DRVFAN_exit();
		thw_drv_cem_fan_pwm_isRunning = false;
		FDC_test_rx_exit(&fdc1_hdl);
		FDC_test_rx_exit(&fdc2_hdl);
		if(	thw_drv_cem_can_tsk_isRunning == true){
			osThreadTerminate(thw_drv_cem_can_tsk_id);
			thw_drv_cem_can_tsk_isRunning = false;
			thw_drv_cem_can_tsk_isSending = false;
		}

		// Return to the previous menu
		thw_drv_setActive();
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_RefreshFn(void)
{
	// Display CAN1 statistics
	THW_goto(17,22);	(thw_drv_cem_can_isComActiv)? THW_printf("Activ"VT100_CLEAREOL): THW_printf("Inactiv"VT100_CLEAREOL);
	THW_goto(18,22);	THW_printf("%d"VT100_CLEAREOL, thw_drv_cem_can_rx_qty);
	THW_goto(19,22);	THW_printf("%d"VT100_CLEAREOL, pThw_drv_cem_can_stat->cpt_sendMsgToCanError_cb);
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_adc_startStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_adc_startStop(void)
{
	// Toggle
	thw_drv_cem_adcConvIsRunning = !thw_drv_cem_adcConvIsRunning;

	if(thw_drv_cem_adcConvIsRunning == true){
		DRVADC_startAdc();
	}else{
		DRVADC_stopAdc();
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_evp_startStop(thw_drv_evp_item_t *pEvp)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_evp_startStop(thw_drv_cem_evp_item_t *pEvp)
{
	// Get Evp running status
	bool isRunning = EVP_isEvpRunning(pEvp->evpId);

	// Toggle
	isRunning = !isRunning;

	// Start/Stop PWM
	if(isRunning){
		EVP_startEvp(pEvp->evpId);
	}else{
		EVP_stopEvp(pEvp->evpId);
	}

}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_evs_pwmStartStop(thw_drv_evs_item_t *pEvs)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_evs_pwmStartStop(thw_drv_cem_evs_item_t *pEvs)
{
	EVS_setFrequencyInKHz(thw_drv_cem_evs_commonFreqInKhz);
	EVS_setEvsDutyCycle(pEvs->evsId, pEvs->dutyCycle);

	// Get Evs running status
	bool isRunning = EVS_isEvsRunning(pEvs->evsId);

	// Toggle
	isRunning = !isRunning;

	// Start/Stop PWM
	if(isRunning){
		EVS_startEvs(pEvs->evsId);
	}else{
		EVS_stopEvs(pEvs->evsId);
	}

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_evAll_StartStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_evAll_StartStop(void)
{
	thw_drv_cem_evp_startStop(&thw_drv_cem_evp1);
	thw_drv_cem_evp_startStop(&thw_drv_cem_evp2);
	thw_drv_cem_evp_startStop(&thw_drv_cem_evp3);
	thw_drv_cem_evs_pwmStartStop(&thw_drv_cem_evs1);
	thw_drv_cem_evs_pwmStartStop(&thw_drv_cem_evs2);
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_pump_StartStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_pump_StartStop(void)
{
	// Toggle
	thw_drv_cem_pump_isActiv = !thw_drv_cem_pump_isActiv;

	// Start/Stop pump
	if(thw_drv_cem_pump_isActiv){
		FDC_test_tx_sendMsgToCan(	&fdc2_hdl,
									thw_drv_cem_can_pump_go.id,
									thw_drv_cem_can_pump_go.size,
									thw_drv_cem_can_pump_go.data);
	}else{
		FDC_test_tx_sendMsgToCan(	&fdc2_hdl,
									thw_drv_cem_can_pump_stop.id,
									thw_drv_cem_can_pump_stop.size,
									thw_drv_cem_can_pump_stop.data);
	}

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_fan_pwmStartStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_fan_pwmStartStop(void)
{
	// Toggle
	thw_drv_cem_fan_pwm_isRunning = !thw_drv_cem_fan_pwm_isRunning;

	// Start/Stop PWM
	if(thw_drv_cem_fan_pwm_isRunning){
		DRVFAN_startPwm();
	}else{
		DRVFAN_stopPwm();
	}

}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_can_tsk_fn(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_can_tsk_fn(void *argument)
{
	osStatus_t 			status;
	static uint32_t 	lastTick = 0;
	static uint32_t 	comActiv_lastTick = 0;

	while(1)
	{
		// RX
		thw_drv_cem_can_newRxMsg = false;;
		status = osMessageQueueGet(fdc1_hdl.prs.mq.id, &thw_drv_cem_can_msg, NULL, THW_DRV_CEM_CAN_SENDING_PERIOD);   // wait for message
		if (status == osOK){
			// Stat
			thw_drv_cem_can_msg.RxTimestamp = HAL_GetTick();
			thw_drv_cem_can_rx_qty++;
			thw_drv_cem_can_newRxMsg = true;
		}

		uint32_t actualTick = HAL_GetTick();

		// Com Activ??
		if(thw_drv_cem_can_newRxMsg){
			thw_drv_cem_can_isComActiv = true;
			comActiv_lastTick = actualTick;
		}else{
			if(actualTick - comActiv_lastTick > 500)
				thw_drv_cem_can_isComActiv = false;
		}

		// TX
		if(actualTick - lastTick >= THW_DRV_CEM_CAN_SENDING_PERIOD){
			lastTick = actualTick;
			// Sending
			if(thw_drv_cem_can_tsk_isSending){
				FDC_test_tx_sendMsgToCan(	&fdc1_hdl,
											thw_drv_cem_can_frameItem.id,
											thw_drv_cem_can_frameItem.size,
											thw_drv_cem_can_frameItem.data);
				thw_drv_cem_can_newTxMsg_TS = HAL_GetTick();
				thw_drv_cem_can_tx_qty++;
				thw_drv_cem_can_newTxMsg = true;
			}
		}

	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_cem_can_startStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_cem_can_startStop(void)
{
	if(	thw_drv_cem_can_tsk_isSending == false){
		thw_drv_cem_can_tsk_isSending = true;
	}else{
		thw_drv_cem_can_tsk_isSending = false;
	}
}


#endif //MODE_THW
