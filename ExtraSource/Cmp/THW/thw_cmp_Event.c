
/*
 * thw_cmp_Event.c
 *
 *  Created on: 13 Dec 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Evt\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#include <stdio.h>

#ifdef MODE_THW
#include <app_configFdc.h>
#include <FDC/fdc_api.h>
#include <stdlib.h>

#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include "fdc_config.h"


const char thw_cmp_evt_menuName[] = "Component | EVT";


static void thw_cmp_evt_periodic_identification(void);
static void thw_cmp_evt_periodic_diag_cpu(void);
static void thw_cmp_evt_periodic_diag_adc(void);
static void thw_cmp_evt_instant_regulAspi_Pressure(void);
static void thw_cmp_evt_simul_regulAspi_Pressure(void);

st_thw_menuItem thw_cmp_evt_menuTab[] = {
		{.name = "Periodic - Identification",			.pActionFn = thw_cmp_evt_periodic_identification, 		.info = 0},
		{.name = "Periodic - Diag - Cpu", 				.pActionFn = thw_cmp_evt_periodic_diag_cpu, 			.info = 0},
		{.name = "Periodic - Diag - Adc", 				.pActionFn = thw_cmp_evt_periodic_diag_adc, 			.info = 0},
		{.name = "Instant  - RegulAspi - Pressure", 	.pActionFn = thw_cmp_evt_instant_regulAspi_Pressure,	.info = 0},
		{.name = "Simul    - RegulAspi - Pressure", 	.pActionFn = thw_cmp_evt_simul_regulAspi_Pressure, 		.info = 0},
};
uint16_t thw_cmp_evt_menuTabSize = sizeof(thw_cmp_evt_menuTab) / sizeof(st_thw_menuItem);

static uint16_t thw_cmp_evt_cpt = 0;


static bool thw_cmp_evt_simul_regulAspi_Pressure_isActive = false;


//-----------------------------------------------------------------------------
// THREAD
//-----------------------------------------------------------------------------
osThreadId_t thw_cmp_evt__tskEvent_id;
const osThreadAttr_t thw_cmp_evt__tskEvent_attr = {
		.name = "thw_cmp_evt__tskEvent",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static void thw_cmp_evt_tskfn(void *arg);




//------------------------------------------------------------------------------
static void thw_cmp_evt_DisplayMenu(void);
static void thw_cmp_evt_ManageChoice(char CodeToManage);
static void thw_cmp_evt_RefreshFn(void);




//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_evt_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_evt_setActive(void)
{
	thw_cmp_evt_cpt = 0;

	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;

	// Initialize Component
	if(EVT_init() != evt_error_NoError)
		return;

	// Start Task
	thw_cmp_evt__tskEvent_id = osThreadNew(thw_cmp_evt_tskfn, NULL, &thw_cmp_evt__tskEvent_attr);

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_cmp_evt_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_cmp_evt_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_cmp_evt_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_evt_menuName);
	THW_printf("\r\n");
	THW_printf("Refresh Running:\r\n");
	THW_printf("\r\n");
	THW_printf("Periodic Event:\r\n");
	THW_printf(" - SE - Identification              :\r\n");
	THW_printf(" - Component - Diag - CpuLoad       :\r\n");
	THW_printf(" - Component - Diag - Adc           :\r\n");
	THW_printf("\r\n");
	THW_printf("Instant Event:\r\n");
	THW_printf(" - Component - RegulAspi - Pressure :\r\n");
	THW_printf("\r\n");
	THW_printf("Simul Instant Event:\r\n");
	THW_printf(" - Component - RegulAspi - Pressure :\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");
	THW_printf("CAN Stat:\r\n");
	THW_printf(" - Sending Ok/Ko   :\r\n");
	THW_printf(" - cpt_rx          :\r\n");
	THW_printf(" - cpt_tx          :\r\n");
	THW_printf(" - ErrorCounters   :\r\n");
	THW_printf(" - ProtocolStatus  :\r\n");
	THW_printf(" - Others          :\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_evt_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_evt_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_evt_menuTabSize)){
		if(thw_cmp_evt_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_evt_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		osThreadTerminate(thw_cmp_evt__tskEvent_id);

		FDC_exit(&fdc1_hdl);
		EVT_exit();

		// Return to the previous menu
		thw_cmp_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_cmp_evt_RefreshFn(void)
{
	thw_cmp_evt_cpt++;

	THW_goto(3, 18);
	THW_printf("%d"VT100_CLEAREOL, thw_cmp_evt_cpt);

	THW_goto(6, 40);
	EVT_isEventActive(IDCMP_SYST_MANAGER, evt_sysmgr_eventId__identification)?
			THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);
	THW_goto(7, 40);
	EVT_isEventActive(IDCMP_DIAG, EVENT_ID_Diag_Cpu)?
			THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);
	THW_goto(8, 40);
	EVT_isEventActive(IDCMP_DIAG, EVENT_ID_Diag_AdcStatus)?
			THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);
	THW_goto(11, 40);
	EVT_isEventActive(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg)?
			THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);
	THW_goto(14, 40);
	thw_cmp_evt_simul_regulAspi_Pressure_isActive?
			THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);

	// CAN Stat
	fdc_Stats_counters_st *pStats = FDC_getStat(&fdc1_hdl);
	THW_goto(18, 22);
	THW_printf("%d/%d"VT100_CLEAREOL, 	pStats->cpt_sendMsgToCanOK_cb,
										pStats->cpt_sendMsgToCanError_cb);
	THW_goto(19, 22);
	THW_printf("%d %d %d %d %d %d"VT100_CLEAREOL,
			pStats->cpt_itRx,
			pStats->cpt_taskRx_threadFlag,
			pStats->cpt_taskRx_ParserMsg_sendOk,
			pStats->cpt_taskRx_ParserMsg_sendKo,
			pStats->cpt_taskRx_BridgeMsg_sendOk,
			pStats->cpt_taskRx_BridgeMsg_sendKo
	);
	THW_goto(20, 22);
	THW_printf("%d %d %d"VT100_CLEAREOL,
			pStats->cpt_taskTx_total,
			pStats->cpt_taskTx_sendOk,
			pStats->cpt_taskTx_sendKo
	);
	THW_goto(21, 22);
	THW_printf("%d %d %d %d"VT100_CLEAREOL,
			pStats->ErrorCounters.TxErrorCnt,
			pStats->ErrorCounters.RxErrorCnt,
			pStats->ErrorCounters.RxErrorPassive,
			pStats->ErrorCounters.ErrorLogging
	);
	THW_goto(22, 22);
	THW_printf("%d %d %d %d %d %d %d %d %d %d %d"VT100_CLEAREOL,
			pStats->ProtocolStatus.Activity,
			pStats->ProtocolStatus.BusOff,
			pStats->ProtocolStatus.DataLastErrorCode,
			pStats->ProtocolStatus.ErrorPassive,
			pStats->ProtocolStatus.LastErrorCode,
			pStats->ProtocolStatus.ProtocolException,
			pStats->ProtocolStatus.RxBRSflag,
			pStats->ProtocolStatus.RxESIflag,
			pStats->ProtocolStatus.RxFDFflag,
			pStats->ProtocolStatus.TDCvalue,
			pStats->ProtocolStatus.Warning
	);
	THW_goto(23, 22);
	THW_printf("%d %d %d"VT100_CLEAREOL,
			pStats->cpt_err_cb,
			pStats->cpt_sendToTxMq_Ok,
			pStats->cpt_sendToTxMq_Ko
	);
}




//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_periodic_identification(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_periodic_identification(void)
{
	if(EVT_isEventActive(IDCMP_SYST_MANAGER, evt_sysmgr_eventId__identification)){
		EVT_Event_disable(IDCMP_SYST_MANAGER, evt_sysmgr_eventId__identification);
	}else{
		EVT_Event_enable(IDCMP_SYST_MANAGER, evt_sysmgr_eventId__identification, 100);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_periodic_diag_cpu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_periodic_diag_cpu(void)
{
	if(EVT_isEventActive(IDCMP_DIAG, EVENT_ID_Diag_Cpu)){
		EVT_Event_disable(IDCMP_DIAG, EVENT_ID_Diag_Cpu);
	}else{
		EVT_Event_enable(IDCMP_DIAG, EVENT_ID_Diag_Cpu, 100);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_periodic_diag_cpu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_periodic_diag_adc(void)
{
	if(EVT_isEventActive(IDCMP_DIAG, EVENT_ID_Diag_AdcStatus)){
		EVT_Event_disable(IDCMP_DIAG, EVENT_ID_Diag_AdcStatus);
	}else{
		EVT_Event_enable(IDCMP_DIAG, EVENT_ID_Diag_AdcStatus, 100);
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_instant_regulAspi_Pressure(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_instant_regulAspi_Pressure(void)
{
	if(EVT_isEventActive(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg)){
		EVT_Event_disable(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg);
	}else{
		EVT_Event_enable(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg, 0);
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_evt_simul_regulAspi_Pressure(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_evt_simul_regulAspi_Pressure(void)
{
	thw_cmp_evt_simul_regulAspi_Pressure_isActive = !thw_cmp_evt_simul_regulAspi_Pressure_isActive;
}


//=============================================================================
//						Thread Function
//=============================================================================
static void thw_cmp_evt_cmp_regulAspi_Pressure_tskfn(void);

static void thw_cmp_evt_tskfn(void *argument)
{
	while(1)
	{
		if(thw_cmp_evt_simul_regulAspi_Pressure_isActive)
			thw_cmp_evt_cmp_regulAspi_Pressure_tskfn();

		osDelay(10);
	}
}


static void thw_cmp_evt_cmp_regulAspi_Pressure_tskfn(void)
{
	static uint16_t 	reg_SetPoint = 0;

	uint8_t nIndex = 0;
	uint8_t DataFrame[12];

	double sinValue = 500.0 * sin(0.1 * reg_SetPoint) + 500.0;
	uint16_t fake_consigne = (uint16_t)sinValue;
	uint16_t noise = rand() % 50; // Ajoute un bruit entre 0 et 50
	uint16_t fake_measure = fake_consigne + noise;

	DataFrame[nIndex++] = (uint8_t)(fake_consigne >> 8);
	DataFrame[nIndex++] = (uint8_t)(fake_consigne);
	DataFrame[nIndex++] = (uint8_t)(fake_measure >> 8);
	DataFrame[nIndex++] = (uint8_t)(fake_measure);
	EVT_Event_rise(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg, DataFrame, nIndex);

	reg_SetPoint++;
}

#endif //MODE_THW
