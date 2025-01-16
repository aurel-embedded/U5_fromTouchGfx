/*
 * thw_drv_evs.c
 *
 *  Created on: 08 fev 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * les EVS\n
 *
 */
#ifdef MODE_THW

#include <string.h>
#include <cmsis_os2.h>
#include "APP.h"
#include <stddef.h>
#include <THW_core/THW_testHardware_common.h>
#include <THW/thw.h>
#include "tim.h"
#include <REG/EVS/evs_api.h>


uint16_t	thw_drv_evs_commonFreqInKhz = 100;
typedef struct{
	evs_id_t		evsId;			// Evs ID
	uint8_t			dutyCycle;		// Duty Cycle in %
	bool			toggleMinMax;			// Toggle Min Max Duty Cycle
}thw_drv_evs_item_t;

thw_drv_evs_item_t thw_drv_evs1 = {.evsId = evs_id1, .dutyCycle = 30, .toggleMinMax = true};
thw_drv_evs_item_t thw_drv_evs2 = {.evsId = evs_id2, .dutyCycle = 50, .toggleMinMax = true};

const char thw_drv_evs_menuName[] = "Driver | EVS";



//-------------------------
static void thw_drv_evs_freqInc(void);
static void thw_drv_evs_freqDec(void);

static void thw_drv_evs_pwmStartStop(thw_drv_evs_item_t *pEvs);
static void thw_drv_evs_dcMinMax(thw_drv_evs_item_t *pEvs);
static void thw_drv_evs_dcInc(thw_drv_evs_item_t *pEvs);
static void thw_drv_evs_dcDec(thw_drv_evs_item_t *pEvs);

static void thw_drv_evs1_pwmStartStop(void){thw_drv_evs_pwmStartStop(&thw_drv_evs1);};
static void thw_drv_evs2_pwmStartStop(void){thw_drv_evs_pwmStartStop(&thw_drv_evs2);};

static void thw_drv_evs1_dcMinMax(void){thw_drv_evs_dcMinMax(&thw_drv_evs1);};
static void thw_drv_evs2_dcMinMax(void){thw_drv_evs_dcMinMax(&thw_drv_evs2);};

static void thw_drv_evs1_dcInc(void){thw_drv_evs_dcInc(&thw_drv_evs1);};
static void thw_drv_evs2_dcInc(void){thw_drv_evs_dcInc(&thw_drv_evs2);};

static void thw_drv_evs1_dcDec(void){thw_drv_evs_dcDec(&thw_drv_evs1);};
static void thw_drv_evs2_dcDec(void){thw_drv_evs_dcDec(&thw_drv_evs2);};


st_thw_menuItem thw_drv_evs_menuTab[] = {
		{.name = "Common : freq ++",				.pActionFn = thw_drv_evs_freqInc, 			.info = 0},
		{.name = "Common : freq --",				.pActionFn = thw_drv_evs_freqDec, 			.info = 0},
		{.name = "EVS1   : start/stop",				.pActionFn = thw_drv_evs1_pwmStartStop, 	.info = 0},
		{.name = "EVS1   : Duty Cycle ++", 			.pActionFn = thw_drv_evs1_dcInc, 			.info = 0},
		{.name = "EVS1   : Duty Cycle --", 			.pActionFn = thw_drv_evs1_dcDec, 			.info = 0},
		{.name = "EVS1   : Duty Cycle Min/Max",		.pActionFn = thw_drv_evs1_dcMinMax, 		.info = 0},
		{.name = "EVS2   : start/stop",				.pActionFn = thw_drv_evs2_pwmStartStop, 	.info = 0},
		{.name = "EVS2   : Duty Cycle ++", 			.pActionFn = thw_drv_evs2_dcInc, 			.info = 0},
		{.name = "EVS2   : Duty Cycle --", 			.pActionFn = thw_drv_evs2_dcDec, 			.info = 0},
		{.name = "EVS2   : Duty Cycle Min/Max",		.pActionFn = thw_drv_evs2_dcMinMax, 		.info = 0},
};
uint16_t thw_drv_evs_menuTabSize = sizeof(thw_drv_evs_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_evs_DisplayMenu(void);
static void thw_drv_evs_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_evs_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_evs_setActive(void)
{
	// Init driver
	EVS_init();

	// Configure driver
	EVS_setFrequencyInKHz(thw_drv_evs_commonFreqInKhz);
	EVS_setEvsDutyCycle(evs_id1, thw_drv_evs1.dutyCycle);
	EVS_setEvsDutyCycle(evs_id2, thw_drv_evs2.dutyCycle);

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_evs_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_evs_ManageChoice;
	thw_actualMenu.refreshFn = 			NULL;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_evs_menuName);
	THW_printf("\r\n");
	THW_printf("Freq min/max       : 10/200 kHz\r\n");
	THW_printf("Freq actual        : %d kHz\r\n", 	 EVS_getFrequencyInKHz());
	THW_printf("Duty Cycle min/max : %d%%/%d%%\r\n", EVS_getEvsDutyCycleMin(), EVS_getEvsDutyCycleMax());
	THW_printf("\r\n");
	THW_printf("EVS1\r\n");
	THW_printf("  Status     : "); (EVS_isEvsRunning(evs_id1))? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Duty Cycle : %d (%d)%%\r\n",	EVS_getEvsDutyCycle(evs_id1), thw_drv_evs1.dutyCycle);
	THW_printf("\r\n");
	THW_printf("EVS2\r\n");
	THW_printf("  Status     : "); (EVS_isEvsRunning(evs_id2))? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Duty Cycle : %d (%d)%%\r\n",	EVS_getEvsDutyCycle(evs_id2), thw_drv_evs2.dutyCycle);
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_evs_menuTab, thw_drv_evs_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_evs_menuTabSize)){
		if(thw_drv_evs_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_evs_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		EVS_stopEvs(evs_id1);
		EVS_stopEvs(evs_id2);

		// Return to the previous menu
		thw_drv_setActive();
	}
}


//*************************************************************************************************
//*************************************************************************************************
// 										TESTING FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************


//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_freqInc(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_freqInc(void)
{
	if(thw_drv_evs_commonFreqInKhz < 200){
		thw_drv_evs_commonFreqInKhz = thw_drv_evs_commonFreqInKhz + 10;
		EVS_setFrequencyInKHz(thw_drv_evs_commonFreqInKhz);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_freqDec(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_freqDec(void)
{
	if(thw_drv_evs_commonFreqInKhz > 10){
		thw_drv_evs_commonFreqInKhz = thw_drv_evs_commonFreqInKhz - 10;
		EVS_setFrequencyInKHz(thw_drv_evs_commonFreqInKhz);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_pwmStartStop(thw_drv_evs_item_t *pEvs)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_pwmStartStop(thw_drv_evs_item_t *pEvs)
{
	EVS_setFrequencyInKHz(thw_drv_evs_commonFreqInKhz);
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
/// \fn 		void thw_drv_evs_fullStartStop(thw_drv_evs_item_t *pEvs)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_dcMinMax(thw_drv_evs_item_t *pEvs)
{
	EVS_stopEvs(pEvs->evsId);


	// Start/Stop PWM
	if(pEvs->toggleMinMax){
		EVS_setEvsDutyCycle(pEvs->evsId, 100);
		EVS_startEvs(pEvs->evsId);
	}else{
		EVS_setEvsDutyCycle(pEvs->evsId, 0);
		EVS_startEvs(pEvs->evsId);
	}

	// Toggle
	pEvs->toggleMinMax = !(pEvs->toggleMinMax);
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs_dcInc(thw_drv_evs_item_t *pEvs)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_dcInc(thw_drv_evs_item_t *pEvs)
{
	if(pEvs->dutyCycle < 100){
		pEvs->dutyCycle = pEvs->dutyCycle + 5;
		EVS_setEvsDutyCycle(pEvs->evsId, pEvs->dutyCycle);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evs1_dcDec(thw_drv_evs_item_t *pEvs)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evs_dcDec(thw_drv_evs_item_t *pEvs)
{
	if(pEvs->dutyCycle > 0){
		pEvs->dutyCycle = pEvs->dutyCycle - 5;
		EVS_setEvsDutyCycle(pEvs->evsId, pEvs->dutyCycle);
	}
}


#endif //MODE_THW
