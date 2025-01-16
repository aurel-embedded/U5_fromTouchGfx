
/*
 * thw_menuCmpReg.c
 *
 *  Created on: 05 Avr 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test hard concernant
 * le composant Reg\n
 * Ces fonctions sont appelées par le composant principal THW.\n\n
 */
#ifdef MODE_THW

#include <app_configFdc.h>
#include <EVT/evt_ID_config.h>
#include <FDC/fdc_api.h>
#include <stdlib.h>
#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <stm32h7xx.h>
#include <stm32h7xx_hal_def.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include "REG/reg_regulation_api.h"
#include "fdc_config.h"
#include <drvAdc/drvAdc_api.h>
#include <EVT/evt_api.h>


const char thw_cmp_reg_menuName[] = "Component | REG";

static void thw_cmp_reg_toggleRegulation(void);
static void thw_cmp_reg_toggleMode(void);
static void thw_cmp_reg_toggleArm(void);
static void thw_cmp_reg_toggleTrajectoryCanFrame(void);
static void thw_cmp_reg_toggleSimulTrajectoryCanFrame(void);
static void thw_cmp_reg_AdcToggleExtIntVref(void);
static void thw_cmp_reg_PumpToggleStartStop_CAN1(void);
static void thw_cmp_reg_PumpToggleStartStop_CAN2(void);
static void thw_cmp_reg_PumpToggleEnableDisable(bool *pEnable, fdc_hdl_t *pCanHdl);

st_thw_menuItem thw_cmp_reg_menuTab[] = {
		{.name = "Toggle Regulation", 						.pActionFn = thw_cmp_reg_toggleRegulation, 				.info = 0},
		{.name = "Toggle Mode", 							.pActionFn = thw_cmp_reg_toggleMode, 					.info = 0},
		{.name = "Toggle Arm", 								.pActionFn = thw_cmp_reg_toggleArm, 					.info = 0},
		{.name = "CAN - Toggle Output", 					.pActionFn = thw_cmp_reg_toggleTrajectoryCanFrame, 		.info = 0},
		{.name = "CAN - Toggle Simul Output", 				.pActionFn = thw_cmp_reg_toggleSimulTrajectoryCanFrame, .info = 0},
		{.name = "ADC - Toggle Ext/Int Vref", 				.pActionFn = thw_cmp_reg_AdcToggleExtIntVref, 			.info = 0},
		{.name = "PUMP - Sending Start/Stop on CAN1", 		.pActionFn = thw_cmp_reg_PumpToggleStartStop_CAN1, 		.info = 0},
		{.name = "PUMP - Sending Start/Stop on CAN2", 		.pActionFn = thw_cmp_reg_PumpToggleStartStop_CAN2, 		.info = 0},
};
uint16_t thw_cmp_reg_menuTabSize = sizeof(thw_cmp_reg_menuTab) / sizeof(st_thw_menuItem);


//bool thw_cmp_reg_regulationEnable = false;
//bool thw_cmp_reg_trajectoryCanFrameEnable = false;
//bool thw_cmp_reg_Adc_useInternalVref = true;

static eXCtrlVibrationMode thw_cmp_reg_vibration = eXCtrlVibrationMode_Off;
static uint32_t thw_cmp_reg_pressureMinMb;
static uint32_t thw_cmp_reg_pressureMaxMb;
static uint32_t thw_cmp_reg_Freq;
static uint32_t thw_cmp_reg_rdc;
static uint32_t thw_cmp_reg_smooth;
static uint32_t thw_cmp_reg_nbOndulations;
static uint32_t thw_cmp_reg_pressureOndulationMb;
static uint8_t thw_cmp_reg_cpt;
static bool thw_cmp_reg_SimulTrajectoryCanFrame_Enable = false;


//-----------------------------------------------------------------------------
// THREAD
//-----------------------------------------------------------------------------
osThreadId_t thw_cmp_reg__tsk1_id;
const osThreadAttr_t thw_cmp_reg__tsk1_attr = {
		.name = "thw_cmp_reg__tsk1",
		.stack_size = 256*8,
		.priority = (osPriority_t)osPriorityNormal,
};
static void thw_cmp_reg__tskfn(void *arg);



//-----------------------------------------------------------------------------
//									PUMP
//-----------------------------------------------------------------------------
bool thw_cmp_reg_CAN1_Pump_isEnable = false;
bool thw_cmp_reg_CAN2_Pump_isEnable = false;
typedef struct{
	uint16_t	id;
	uint8_t 	data[10];
	uint8_t 	size;
}thw_cmp_reg_can_frameItem_t;
static thw_cmp_reg_can_frameItem_t thw_cmp_reg_can_pump_go = {
		.id = 	0x293,
		.data = {0x01, 0x01},
		.size = 2,
};
static thw_cmp_reg_can_frameItem_t thw_cmp_reg_can_pump_stop = {
		.id = 	0x293,
		.data = {0x00, 0x00},
		.size = 2,
};


//------------------------------------------------------------------------------
static void thw_cmp_reg_DisplayMenu(void);
static void thw_cmp_reg_ManageChoice(char CodeToManage);
static void thw_cmp_reg_RefreshFn(void);




//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_cmp_reg_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_cmp_reg_setActive(void)
{
	thw_cmp_reg_cpt = 0;

	// Initialize Component
	if(FDC_init(&fdc1_hdl, fdc1_filterTab, fdc1_filterTab_size) != HAL_OK)
		return;

	// Initialize Component
	if(FDC_init(&fdc2_hdl, fdc2_filterTab, fdc2_filterTab_size) != HAL_OK)
		return;

	// Initialize Component
	if(REG_init() != HAL_OK)
		return;

	// Initialize Component
	if(EVT_init() != evt_error_NoError)
		return;



	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_cmp_reg_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_cmp_reg_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_cmp_reg_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_DisplayMenu(void)
{

	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_cmp_reg_menuName);
	THW_printf("\r\n");
	THW_printf("Refresh Running:\r\n");
	THW_printf("\r\n");
	THW_printf("Regulation\r\n");
	THW_printf("Enable              :\r\n");
	THW_printf("Mode                :\r\n");
	THW_printf("Arm                 :\r\n");
	THW_printf("Pmin / Pmax         :\r\n");
	THW_printf("Freq, Rdc           :\r\n");
	THW_printf("Vibration           :\r\n");
	THW_printf("Smooth, Rip nbr/lvl :\r\n");
	THW_printf("Event               :\r\n");
	THW_printf("Simul Event         :\r\n");
	THW_printf("\r\n");
	THW_printf("ADC - Vref          :\r\n");
	THW_printf("\r\n");
	THW_printf("PUMP - CAN1: ");(thw_cmp_reg_CAN1_Pump_isEnable)? THW_printf("Run\r\n"): THW_printf("Stop\r\n");
	THW_printf("PUMP - CAN2: ");(thw_cmp_reg_CAN2_Pump_isEnable)? THW_printf("Run\r\n"): THW_printf("Stop\r\n");
	THW_printf("\r\n");
//	THW_printf("Trajectory Mode Selection:\r\n");
//	thw_cmp_reg_displayModeSelection();
	THW_printf("\r\n");

	// Affichage du menu
	for(uint16_t ind = 0; ind < thw_cmp_reg_menuTabSize; ind++){
		THW_printf("%2d - %s\r\n",
				ind + 1,
				thw_cmp_reg_menuTab[ind].name);
	}
	THW_printf("\r\n");
	THW_printf("%2d - Retour\r\n", 		thw_cmdRetour);
	THW_printf("\r\n");
	THW_printf("Choix :  ");
	THW_saveCurPos();

}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_cmp_reg_menuTabSize)){
		if(thw_cmp_reg_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_cmp_reg_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		if(thw_cmp_reg__tsk1_id != NULL){
			osThreadTerminate(thw_cmp_reg__tsk1_id);
			thw_cmp_reg_SimulTrajectoryCanFrame_Enable = false;
		}

		FDC_exit(&fdc1_hdl);
		FDC_exit(&fdc2_hdl);
		REG_exit();
		EVT_exit();

		// Return to the previous menu
		thw_cmp_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_cmp_reg_RefreshFn(void)
{
	thw_cmp_reg_cpt++;
	bool regActivationStatus = REG_getStatus();
	REG_Trajectory_Vibration_get(&thw_cmp_reg_vibration);
	REG_Trajectory_RythmiciteEx_PressureMinMax_get(&thw_cmp_reg_pressureMinMb, &thw_cmp_reg_pressureMaxMb);
	thw_cmp_reg_Freq = REG_Trajectory_RythmiciteEx_Freq_get();
	thw_cmp_reg_rdc = REG_Trajectory_RythmiciteEx_Rdc_get();
	thw_cmp_reg_smooth = REG_Trajectory_RythmiciteEx_Smooth_get();
	REG_Trajectory_RythmiciteEx_Ondulation_get(&thw_cmp_reg_nbOndulations, &thw_cmp_reg_pressureOndulationMb);
	bool TrajectoryCanFrameSending = EVT_isEventActive(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg);

	THW_goto(3, 18);
	THW_printf("%d"VT100_CLEAREOL, thw_cmp_reg_cpt);

	THW_goto(6, 23);
	regActivationStatus? THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);

	THW_goto(7, 23);
	THW_printf("%s"VT100_CLEAREOL, REG_Trajectory_Mode_getStr());

	THW_goto(8, 23);
	(REG_SelectArm_get() == reg_arm_1)? THW_printf("Arm 1"VT100_CLEAREOL): THW_printf("Arm 2"VT100_CLEAREOL);

	THW_goto(9, 23);
	THW_printf("%d mb / %d mb"VT100_CLEAREOL, thw_cmp_reg_pressureMinMb, thw_cmp_reg_pressureMaxMb);

	THW_goto(10, 23);
	THW_printf("%d - %d"VT100_CLEAREOL, thw_cmp_reg_Freq, thw_cmp_reg_rdc);

	THW_goto(11, 23);
	THW_printf("%d"VT100_CLEAREOL, thw_cmp_reg_vibration);

	THW_goto(12, 23);
	THW_printf("%d - %d - %d mb"VT100_CLEAREOL, thw_cmp_reg_smooth, thw_cmp_reg_nbOndulations, thw_cmp_reg_pressureOndulationMb);

	THW_goto(13, 23);
	TrajectoryCanFrameSending? THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);

	THW_goto(14, 23);
	thw_cmp_reg_SimulTrajectoryCanFrame_Enable? THW_printf("Enable"VT100_CLEAREOL): THW_printf("Disable"VT100_CLEAREOL);

	THW_goto(16, 23);
	DRVADC_isUsingVrefInt()? THW_printf("Internal"VT100_CLEAREOL): THW_printf("External"VT100_CLEAREOL);
}




//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_toggleRegulation(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_toggleRegulation(void)
{
	if(REG_getStatus())
		REG_Stop();
	else
		REG_Start();
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_toggleMode(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_toggleMode(void)
{
	switch (REG_Trajectory_Mode_get()) {
		case reg_trajMode_undef:
			REG_Trajectory_Mode_set(reg_trajMode_rythmicite_ex);
			break;

		case reg_trajMode_rythmicite_ex:
			REG_Trajectory_Mode_set(reg_trajMode_rythmicite_ex_sync);
			break;

		case reg_trajMode_rythmicite_ex_sync:
			REG_Trajectory_Mode_set(reg_trajMode_undef);
			break;

		default:
			REG_Trajectory_Mode_set(reg_trajMode_undef);
			break;
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_toggleArm(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_toggleArm(void)
{
	if(REG_SelectArm_get() == reg_arm_1)
		REG_SelectArm_set(reg_arm_2);
	else
		REG_SelectArm_set(reg_arm_1);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_toggleTrajectoryCanFrame(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_toggleTrajectoryCanFrame(void)
{
	if(EVT_isEventActive(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg)){
		EVT_Event_disable(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg);
	}else{
		EVT_Event_enable(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg, 0);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_toggleSimulTrajectoryCanFrame(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_toggleSimulTrajectoryCanFrame(void)
{
	thw_cmp_reg_SimulTrajectoryCanFrame_Enable = !thw_cmp_reg_SimulTrajectoryCanFrame_Enable;

	if(thw_cmp_reg_SimulTrajectoryCanFrame_Enable){
		thw_cmp_reg__tsk1_id = osThreadNew(thw_cmp_reg__tskfn, NULL, &thw_cmp_reg__tsk1_attr);
	}else{
		osThreadTerminate(thw_cmp_reg__tsk1_id);
	}
}

////------------------------------------------------------------------------------
///// \fn 		void thw_cmp_reg_toggleTrajectoryMode(void)
///// \brief
////------------------------------------------------------------------------------
//static void thw_cmp_reg_toggleTrajectoryMode(void)
//{
//	thw_cmp_reg_modeItemList_ind = (thw_cmp_reg_modeItemList_ind + 1) % thw_cmp_reg_modeItemList_size;
//}

////------------------------------------------------------------------------------
///// \fn 		void thw_cmp_reg_sendTrajectoryMode(void)
///// \brief
////------------------------------------------------------------------------------
//static void thw_cmp_reg_sendTrajectoryMode(void)
//{
//	REG_Trajectory_set(thw_cmp_reg_modeItemList[thw_cmp_reg_modeItemList_ind].mode);
//}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_AdcToggleExtIntVref(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_AdcToggleExtIntVref(void)
{
	if(DRVADC_isUsingVrefInt())
		DRVADC_selectVref(drvAdc_vref_ext);
	else
		DRVADC_selectVref(drvAdc_vref_int);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_PumpToggleEnableDisable(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_PumpToggleEnableDisable(bool *pEnable, fdc_hdl_t *pCanHdl)
{
	*pEnable = !*pEnable;
	if(*pEnable){
		FDC_test_tx_sendMsgToCan(	pCanHdl,
									thw_cmp_reg_can_pump_go.id,
									thw_cmp_reg_can_pump_go.size,
									thw_cmp_reg_can_pump_go.data);
	}else{
		FDC_test_tx_sendMsgToCan(	pCanHdl,
									thw_cmp_reg_can_pump_stop.id,
									thw_cmp_reg_can_pump_stop.size,
									thw_cmp_reg_can_pump_stop.data);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_PumpToggleEnableDisable_CAN1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_PumpToggleStartStop_CAN1(void)
{
	thw_cmp_reg_PumpToggleEnableDisable(&thw_cmp_reg_CAN1_Pump_isEnable, &fdc1_hdl);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_cmp_reg_PumpToggleEnableDisable_CAN2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_cmp_reg_PumpToggleStartStop_CAN2(void)
{
	thw_cmp_reg_PumpToggleEnableDisable(&thw_cmp_reg_CAN2_Pump_isEnable, &fdc2_hdl);
}

////------------------------------------------------------------------------------
///// \fn 		void thw_drv_cmp_displayModeSelection(void)
///// \brief
////------------------------------------------------------------------------------
//static void thw_cmp_reg_displayModeSelection(void)
//{
//	for(uint8_t i = 0; i < thw_cmp_reg_modeItemList_size; i++){
//		if(i == thw_cmp_reg_modeItemList_ind)
//			THW_printf(" -->");
//		else
//			THW_printf("    ");
//		THW_printf(" %s", thw_cmp_reg_modeItemList[i].name);
//		THW_printf(VT100_CLEAREOL"\r\n");
//	}
//}

//=============================================================================
//						Thread Function
//=============================================================================
static void thw_cmp_reg__tskfn(void *argument)
{
	static uint16_t 	reg_SetPoint = 0;

	while(1)
	{
		uint8_t nIndex = 0;
		uint8_t DataFrame[12];

        double sinValue = 500.0 * sin(0.1 * reg_SetPoint) + 500.0;
        uint16_t fake_consigne = (uint16_t)sinValue;
        uint16_t noise = rand() % 50; // Ajoute un bruit entre 0 et 500
        uint16_t fake_measure = fake_consigne + noise;

        DataFrame[nIndex++] = (uint8_t)(fake_consigne >> 8);
		DataFrame[nIndex++] = (uint8_t)(fake_consigne);
        DataFrame[nIndex++] = (uint8_t)(fake_measure >> 8);
		DataFrame[nIndex++] = (uint8_t)(fake_measure);
		EVT_Event_rise(IDCMP_SRVREGULASPI, EVENT_ID_RegulAspi_trajReg, DataFrame, nIndex);

		reg_SetPoint++;
		osDelay(10);
	}
}

#endif //MODE_THW
