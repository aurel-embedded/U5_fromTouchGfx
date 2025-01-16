/*
 * thw_drv_evp.c
 *
 *  Created on: 08 fev 2023
 *      Author: apajadon
 *
 * Ce sous-composant du composant THW s'occupe de gérer les test drivers concernant
 * les EVP\n
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
#include <REG/EVP/evp_api.h>


uint16_t	thw_drv_evp_commonFreqInKhz = 100;
typedef struct{
	evp_id_t		evpId;					// Evp ID
	uint8_t			requestDutyCycle;		// Duty Cycle in %
}thw_drv_evp_item_t;
thw_drv_evp_item_t thw_drv_evp1 = {.evpId = evp_id1, .requestDutyCycle = 30};
thw_drv_evp_item_t thw_drv_evp2 = {.evpId = evp_id2, .requestDutyCycle = 50};
thw_drv_evp_item_t thw_drv_evp3 = {.evpId = evp_id3, .requestDutyCycle = 70};

const char thw_drv_evp_menuName[] = "Driver | EVP";



//-------------------------
static void thw_drv_evp_freqInc(void);
static void thw_drv_evp_freqDec(void);

static void thw_drv_evp_startStop(thw_drv_evp_item_t *pEvp);
static void thw_drv_evp_dcInc(thw_drv_evp_item_t *pEvp);
static void thw_drv_evp_dcDec(thw_drv_evp_item_t *pEvp);

static void thw_drv_evp1_startStop(void){thw_drv_evp_startStop(&thw_drv_evp1);};
static void thw_drv_evp2_startStop(void){thw_drv_evp_startStop(&thw_drv_evp2);};
static void thw_drv_evp3_startStop(void){thw_drv_evp_startStop(&thw_drv_evp3);};

static void thw_drv_evp1_dcInc(void){thw_drv_evp_dcInc(&thw_drv_evp1);};
static void thw_drv_evp2_dcInc(void){thw_drv_evp_dcInc(&thw_drv_evp2);};
static void thw_drv_evp3_dcInc(void){thw_drv_evp_dcInc(&thw_drv_evp3);};

static void thw_drv_evp1_dcDec(void){thw_drv_evp_dcDec(&thw_drv_evp1);};
static void thw_drv_evp2_dcDec(void){thw_drv_evp_dcDec(&thw_drv_evp2);};
static void thw_drv_evp3_dcDec(void){thw_drv_evp_dcDec(&thw_drv_evp3);};


st_thw_menuItem thw_drv_evp_menuTab[] = {
		{.name = "Common   : freq ++",			.pActionFn = thw_drv_evp_freqInc, 			.info = 0},
		{.name = "Common   : freq --",			.pActionFn = thw_drv_evp_freqDec, 			.info = 0},
		{.name = "EVP1     : start/stop",		.pActionFn = thw_drv_evp1_startStop, 		.info = 0},
		{.name = "EVP1     : Duty Cycle ++", 	.pActionFn = thw_drv_evp1_dcInc, 			.info = 0},
		{.name = "EVP1     : Duty Cycle --", 	.pActionFn = thw_drv_evp1_dcDec, 			.info = 0},
		{.name = "EVP2     : start/stop",		.pActionFn = thw_drv_evp2_startStop, 		.info = 0},
		{.name = "EVP2     : Duty Cycle ++", 	.pActionFn = thw_drv_evp2_dcInc, 			.info = 0},
		{.name = "EVP2     : Duty Cycle --", 	.pActionFn = thw_drv_evp2_dcDec, 			.info = 0},
		{.name = "EVP3     : start/stop",		.pActionFn = thw_drv_evp3_startStop, 		.info = 0},
		{.name = "EVP3     : Duty Cycle ++", 	.pActionFn = thw_drv_evp3_dcInc, 			.info = 0},
		{.name = "EVP3     : Duty Cycle --", 	.pActionFn = thw_drv_evp3_dcDec, 			.info = 0},
};
uint16_t thw_drv_evp_menuTabSize = sizeof(thw_drv_evp_menuTab) / sizeof(st_thw_menuItem);


static void thw_drv_evp_DisplayMenu(void);
static void thw_drv_evp_ManageChoice(char CodeToManage);

//------------------------------------------------------------------------------
// DESCRIPTION:         void thw_drv_evp_setActive(void)
// PARAMETERS:          void
// RETURN VALUE:        void
// DESIGN INFORMATION:
//------------------------------------------------------------------------------
void thw_drv_evp_setActive(void)
{
	// Init driver
	EVP_init();

	// Configure driver
	EVP_setFrequencyInKHz(thw_drv_evp_commonFreqInKhz);
	EVP_setEvpDutyCycle(evp_id1, thw_drv_evp1.requestDutyCycle);
	EVP_setEvpDutyCycle(evp_id2, thw_drv_evp2.requestDutyCycle);
	EVP_setEvpDutyCycle(evp_id3, thw_drv_evp3.requestDutyCycle);

	// Menu affiché
	thw_actualMenu.displayMenu = 		thw_drv_evp_DisplayMenu;
	thw_actualMenu.manageChoiceFn = 	thw_drv_evp_ManageChoice;
	thw_actualMenu.refreshFn = 			NULL;
}


//*************************************************************************************************
//*************************************************************************************************
// 										CORE FUNCTIONS
//*************************************************************************************************
//*************************************************************************************************
//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evp_DisplayMenu(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_DisplayMenu(void)
{
	// RAZ screen
	THW_clearScreen();

	// Affichage Menu
	THW_Banner(thw_drv_evp_menuName);
	THW_printf("\r\n");
	THW_printf("Freq min/max       : 10/200 kHz\r\n");
	THW_printf("Freq actual        : %d kHz\r\n", 	 EVP_getFrequencyInKHz());
	THW_printf("Duty Cycle min/max : %d%%/%d%%\r\n", EVP_getEvpDutyCycleMin(), EVP_getEvpDutyCycleMax());
	THW_printf("\r\n");
	THW_printf("EVP1\r\n");
	THW_printf("  Status     : "); (EVP_isEvpRunning(evp_id1))? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Duty Cycle : %d %%\r\n",	EVP_getEvpDutyCycle(evp_id1));
	THW_printf("\r\n");
	THW_printf("EVP2\r\n");
	THW_printf("  Status     : "); (EVP_isEvpRunning(evp_id2))? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Duty Cycle : %d %%\r\n", 	EVP_getEvpDutyCycle(evp_id2));
	THW_printf("\r\n");
	THW_printf("EVP3\r\n");
	THW_printf("  Status     : "); (EVP_isEvpRunning(evp_id3))? THW_printf("Running\r\n"): THW_printf("Stopped\r\n");
	THW_printf("  Duty Cycle : %d %%\r\n", 	EVP_getEvpDutyCycle(evp_id3));
	THW_printf("\r\n");

	// Affichage du menu
	THW_printf("Menu\r\n");
	THW_displayActionMenu(thw_drv_evp_menuTab, thw_drv_evp_menuTabSize);

}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evp_ManageChoice(char CodeToManage)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_ManageChoice(char CodeToManage)
{
	// Excute the corresponding action
	if((CodeToManage > 0) && (CodeToManage <= thw_drv_evp_menuTabSize)){
		if(thw_drv_evp_menuTab[CodeToManage - 1].pActionFn != NULL)
			thw_drv_evp_menuTab[CodeToManage - 1].pActionFn();
	}
	// Or Return to the previous menu
	else if(CodeToManage == 0){
		EVP_stopEvp(evp_id1);
		EVP_stopEvp(evp_id2);
		EVP_stopEvp(evp_id3);

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
/// \fn 		void thw_drv_evp_freqInc(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_freqInc(void)
{
	if(thw_drv_evp_commonFreqInKhz < 200){
		thw_drv_evp_commonFreqInKhz = thw_drv_evp_commonFreqInKhz + 10;
		EVP_setFrequencyInKHz(thw_drv_evp_commonFreqInKhz);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evp_freqDec(void)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_freqDec(void)
{
	if(thw_drv_evp_commonFreqInKhz > 10){
		thw_drv_evp_commonFreqInKhz = thw_drv_evp_commonFreqInKhz - 10;
		EVP_setFrequencyInKHz(thw_drv_evp_commonFreqInKhz);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evp1_startStop(thw_drv_evp_item_t *pEvp)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_startStop(thw_drv_evp_item_t *pEvp)
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
/// \fn 		void thw_drv_evp_dcInc(thw_drv_evp_item_t *pEvp)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_dcInc(thw_drv_evp_item_t *pEvp)
{
	if(pEvp->requestDutyCycle < EVP_getEvpDutyCycleMax()){
		pEvp->requestDutyCycle = pEvp->requestDutyCycle + 5;
		EVP_setEvpDutyCycle(pEvp->evpId, pEvp->requestDutyCycle);
	}
}

//------------------------------------------------------------------------------
/// \fn 		void thw_drv_evp1_dcDec(thw_drv_evp_item_t *pEvp)
/// \brief
//------------------------------------------------------------------------------
static void thw_drv_evp_dcDec(thw_drv_evp_item_t *pEvp)
{
	if(pEvp->requestDutyCycle > EVP_getEvpDutyCycleMin()){
		pEvp->requestDutyCycle = pEvp->requestDutyCycle - 5;
		EVP_setEvpDutyCycle(pEvp->evpId, pEvp->requestDutyCycle);
	}
}


#endif //MODE_THW
