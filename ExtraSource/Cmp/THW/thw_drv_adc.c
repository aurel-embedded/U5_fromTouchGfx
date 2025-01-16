
/*
 * thw_drv_adc.c
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
#include <FAN/drvTachy/drvTachy.h>
#include <drvAdc/drvAdc_api.h>


const char thw_drv_adc_menuName[] = "Driver | ADC";


//-------------------------
static void thw_drv_adc_startStop(void);
static void thw_drv_adc_toggleVref(void);
static void thw_drv_adc_toggleDepExt1(void);
static void thw_drv_adc_toggleDepExt2(void);
static void thw_drv_adc_toggleDepInt(void);

static void thw_drv_adc_dep1_setOffset(void);
static void thw_drv_adc_toggleTempExt1(void);

st_thw_menuItem thw_drv_adc_menuTab[] = {
		{.name = "ADC       :  start/stop",							.pActionFn = thw_drv_adc_startStop, 		.info = 0},
		{.name = "ADC       :  Vref Int/Ext",						.pActionFn = thw_drv_adc_toggleVref, 		.info = 0},
		{.name = "Dep Ext 1 :  \"4-20mA\"/\"0-5V K\"\"0-5V P\"",	.pActionFn = thw_drv_adc_toggleDepExt1, 	.info = 0},
		{.name = "Dep Ext 2 :  \"4-20mA\"/\"0-5V K\"\"0-5V P\"",	.pActionFn = thw_drv_adc_toggleDepExt2, 	.info = 0},
		{.name = "Dep Int   :  \"Nxp\"/\"HoneyWell\"",				.pActionFn = thw_drv_adc_toggleDepInt, 		.info = 0},
		{.name = "Dep Int   :  Set offset",							.pActionFn = thw_drv_adc_dep1_setOffset, 	.info = 0},
		{.name = "Temp Ext1 :  \"NTC\"/\"PT1000\"",					.pActionFn = thw_drv_adc_toggleTempExt1,	.info = 0},
};
uint16_t thw_drv_adc_menuTabSize = sizeof(thw_drv_adc_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_adc_DisplayMenu(void);
static void thw_drv_adc_ManageChoice(char CodeToManage);
static void thw_drv_adc_RefreshFn(void);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_adc_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_adc_setActive(void)
{
	DRVADC_init();

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_adc_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_adc_ManageChoice;
	thw_actualMenu.refreshFn = 			thw_drv_adc_RefreshFn;
	thw_actualMenu.refreshPeriodInMs = 	100;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_adc_menuName);
	THW_printf("\r\n");

	THW_printf("ADC\r\n");
	THW_printf("  State          : \r\n");
	THW_printf("  Vref           : \r\n");
	THW_printf("  Vref Value     : \r\n");
	THW_printf("  Cpu Temp       :\r\n");
	THW_printf("\r\n");
	THW_printf("                   Register       Adc            Converted      Physical\r\n");
	THW_printf("  3.3 V          :\r\n");
	THW_printf("  12 V           :\r\n");
	THW_printf("  24 V           :\r\n");
	THW_printf("\r\n");
	THW_printf("  Int Temp       :\r\n");
	THW_printf("  Ext Temp  Mode : \"%s\"\r\n", DRVADC_tempExt_getModeStr());
	THW_printf("  Ext Temp       :\r\n");
	THW_printf("\r\n");
	THW_printf("  Dep Ext 1 Mode : \"%s\"\r\n", DRVADC_depExt1_getModeStr());
	THW_printf("  Dep Ext 1      :\r\n");
	THW_printf("  Dep Ext 2 Mode : \"%s\"\r\n", DRVADC_depExt2_getModeStr());
	THW_printf("  Dep Ext 2      :\r\n");
	THW_printf("  Dep Int Mode   : \"%s\"\r\n", DRVADC_depInt_getModeStr());
	THW_printf("  Dep Int        :\r\n");
	THW_printf("\r\n");
	THW_printf("  Imon           :\r\n");
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_adc_menuTab, thw_drv_adc_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_adc_menuTabSize)){
		if(thw_drv_adc_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_adc_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		DRVADC_stopAdc();
		DRVADC_exit();

		// Return to the previous menu
		thw_drv_setActive();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_RefreshFn(void)
/// \brief
//------------------------------------------------------------------------------
void thw_drv_adc_RefreshFn(void)
{
	bool isAdcRunning = DRVADC_isAdcRunning();
	THW_goto(4, 20); 	(isAdcRunning)? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_goto(5, 20); 	(DRVADC_isUsingVrefInt())? THW_printf("Internal\r\n"): THW_printf("External\r\n");
	THW_goto(15, 20); 	THW_printf("\"%s\""VT100_CLEAREOL, DRVADC_tempExt_getModeStr());
	THW_goto(18, 20); 	THW_printf("\"%s\""VT100_CLEAREOL, DRVADC_depExt1_getModeStr());
	THW_goto(20, 20); 	THW_printf("\"%s\""VT100_CLEAREOL, DRVADC_depExt2_getModeStr());
	THW_goto(22, 20); 	THW_printf("\"%s\""VT100_CLEAREOL, DRVADC_depInt_getModeStr());

	if(isAdcRunning){
		drvAdc_adcVal_t adcVal_3v3 = DRVADC_getV_3_3();
		drvAdc_adcVal_t adcVal_v12 = DRVADC_getV_12();
		drvAdc_adcVal_t adcVal_v24 = DRVADC_getV_24();
		drvAdc_adcVal_t adcVal_intTemp = DRVADC_tempInt_getValue();
		drvAdc_adcVal_t adcVal_extTemp = DRVADC_tempExt_getValue();
		drvAdc_adcVal_t adcVal_depExt1 = DRVADC_depExt1_getVal();
		drvAdc_adcVal_t adcVal_depExt2 = DRVADC_depExt2_getVal();
		drvAdc_adcVal_t adcVal_depInt = DRVADC_depInt_getVal();
		drvAdc_adcVal_t adcVal_bras1Imon = DRVADC_bras1Imon_getVal();


		THW_goto(6, 20); 	THW_printf("%d mV"VT100_CLEAREOL, DRVADC_getVref());
		THW_goto(7, 20); 	THW_printf("%d °C"VT100_CLEAREOL, DRVADC_getCpuTemp());

		THW_goto(10, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_3v3.reg);
		THW_goto(11, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_v12.reg);
		THW_goto(12, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_v24.reg);
		THW_goto(14, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_intTemp.reg);
		THW_goto(16, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_extTemp.reg);
		THW_goto(19, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_depExt1.reg);
		THW_goto(21, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_depExt2.reg);
		THW_goto(23, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_depInt.reg);
		THW_goto(25, 20); 	THW_printf("%d"VT100_CLEAREOL, adcVal_bras1Imon.reg);

		THW_goto(10, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_3v3.Vadc);
		THW_goto(11, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_v12.Vadc);
		THW_goto(12, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_v24.Vadc);
		THW_goto(14, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_intTemp.Vadc);
		THW_goto(16, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_extTemp.Vadc);
		THW_goto(19, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_depExt1.Vadc);
		THW_goto(21, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_depExt2.Vadc);
		THW_goto(23, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_depInt.Vadc);
		THW_goto(25, 35); 	THW_printf("%d mV"VT100_CLEAREOL, adcVal_bras1Imon.Vadc);

		THW_goto(10, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_3v3.Sensor);
		THW_goto(11, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_v12.Sensor);
		THW_goto(12, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_v24.Sensor);
		THW_goto(14, 50); 	THW_printf("%.0f Ohms"VT100_CLEAREOL, adcVal_intTemp.Sensor);
		THW_goto(16, 50); 	THW_printf("%.0f Ohms"VT100_CLEAREOL, adcVal_extTemp.Sensor);
		if(DRVADC_depExt1_getMode() == DRVADC_deptExt_mode_420mA){
			THW_goto(19, 50); 	THW_printf("%.0f mA"VT100_CLEAREOL, adcVal_depExt1.Sensor);
		}else{
			THW_goto(19, 50); 	THW_printf("%.2f V"VT100_CLEAREOL, adcVal_depExt1.Sensor);
		}
		if(DRVADC_depExt2_getMode() == DRVADC_deptExt_mode_420mA){
			THW_goto(21, 50); 	THW_printf("%.0f mA"VT100_CLEAREOL, adcVal_depExt2.Sensor);
		}else{
			THW_goto(21, 50); 	THW_printf("%.2f V"VT100_CLEAREOL, adcVal_depExt2.Sensor);
		}
		THW_goto(23, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_depInt.Sensor);
		THW_goto(25, 50); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_bras1Imon.Sensor);

		THW_goto(10, 65); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_3v3.Physical);
		THW_goto(11, 65); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_v12.Physical);
		THW_goto(12, 65); 	THW_printf("%.0f mV"VT100_CLEAREOL, adcVal_v24.Physical);
		THW_goto(14, 65); 	THW_printf("%.0f °C"VT100_CLEAREOL, adcVal_intTemp.Physical);
		THW_goto(16, 65); 	THW_printf("%.0f °C"VT100_CLEAREOL, adcVal_extTemp.Physical);
		THW_goto(19, 65); 	THW_printf("%.2f mbar"VT100_CLEAREOL, adcVal_depExt1.Physical);
		THW_goto(21, 65); 	THW_printf("%.2f mbar"VT100_CLEAREOL, adcVal_depExt2.Physical);
		THW_goto(23, 65); 	THW_printf("%.2f mbar"VT100_CLEAREOL, adcVal_depInt.Physical);
		THW_goto(25, 65); 	THW_printf("%.2f mA"VT100_CLEAREOL, adcVal_bras1Imon.Physical);

	}
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_startStop(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_startStop(void)
{
	// Toggle
	if(DRVADC_isAdcRunning()){
		DRVADC_stopAdc();
	}else{
		DRVADC_startAdc();
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_toggleVref(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_toggleVref(void)
{
	if(DRVADC_isUsingVrefInt() == true){
		DRVADC_selectVref(drvAdc_vref_ext);
	}else{
		DRVADC_selectVref(drvAdc_vref_int);
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_toggleDepExt1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_toggleDepExt1(void)
{
	if(DRVADC_depExt1_getMode() == DRVADC_deptExt_mode_420mA){
		DRVADC_depExt1_setMode(DRVADC_deptExt_mode_05V_K);
	}else if(DRVADC_depExt1_getMode() == DRVADC_deptExt_mode_05V_K){
		DRVADC_depExt1_setMode(DRVADC_deptExt_mode_05V_P);
	}else{
		DRVADC_depExt1_setMode(DRVADC_deptExt_mode_420mA);
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_toggleDepExt2(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_toggleDepExt2(void)
{
	if(DRVADC_depExt2_getMode() == DRVADC_deptExt_mode_420mA){
		DRVADC_depExt2_setMode(DRVADC_deptExt_mode_05V_K);
	}else if(DRVADC_depExt2_getMode() == DRVADC_deptExt_mode_05V_K){
		DRVADC_depExt2_setMode(DRVADC_deptExt_mode_05V_P);
	}else{
		DRVADC_depExt2_setMode(DRVADC_deptExt_mode_420mA);
	}
}


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_toggleDepInt(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_toggleDepInt(void)
{
	if(DRVADC_depInt_getMode() == DRVADC_deptInt_mode_Nxp){
				DRVADC_depInt_setMode(DRVADC_deptInt_mode_HoneyWell_ABP2);
	}else if(DRVADC_depInt_getMode() == DRVADC_deptInt_mode_HoneyWell_ABP2){
		DRVADC_depInt_setMode(DRVADC_deptInt_mode_Nxp);
	}else{
		DRVADC_depInt_setMode(DRVADC_deptInt_mode_Nxp);
	}
}



//------------------------------------------------------------------------------
/// \fn 		void thw_drv_dep1_setOffset(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_dep1_setOffset(void)
{
	DRVADC_depInt_setOffset(DRVADC_depInt_getVal().Physical);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_adc_toggleTempExt1(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_adc_toggleTempExt1(void)
{
	if(DRVADC_tempExt_getMode() == DRVADC_tempExt_mode_NTC){
		DRVADC_tempExt_setMode(DRVADC_tempExt_mode_PT1000);
	}else if(DRVADC_tempExt_getMode() == DRVADC_tempExt_mode_PT1000){
		DRVADC_tempExt_setMode(DRVADC_tempExt_mode_NTC);
	}else{
		DRVADC_tempExt_setMode(DRVADC_tempExt_mode_NTC);
	}
}


#endif //MODE_THW
