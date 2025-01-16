/*
 * pmgr_thread.cpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 *
 *      Design Pattern: Singleton
 *
 */
#include <MDI_midi/mdi_thread.hpp>
#include <Tools/Tools.h>
#include <UserInterfaces/drvAdc/drvAdc.h>
#include <PotarManager/pmgr_thread.hpp>



// Constructeur privé : initialise les membres
pmgr_thread::pmgr_thread() : threadId(nullptr)
{
}

// Destructeur privé : arrête le thread s'il est actif
pmgr_thread::~pmgr_thread()
{
    exit(); // Assurer la terminaison du thread
}

//=============================================================================
//									TASK
//=============================================================================

// Fonction du thread (statique)
void pmgr_thread::threadFunction(void* argument)
{
	pmgr_thread* self  = static_cast<pmgr_thread*>(argument); // Conversion en instance de la classe

	DRVADC_startAdc();

	while (!self->thread_stopRequested)
    {

    	// Manage Component State
    	switch (self->currentState) {
			case FsmState::Idle:
				break;

			case FsmState::Mode1:
				threadFunction_mode1(argument);
				break;

			case FsmState::Error:
				break;

			default:
				break;
		}

    	osDelay(taskDelay);
    }
}

//=============================================================================
//								Private Methods
//=============================================================================

// Fonction du thread (statique)
void pmgr_thread::threadFunction_mode1(void* argument)
{
	userTypes_6Uint8_t midiValues;
	static userTypes_6Uint8_t midiValues_old;

	// Get ADC Values
	//----------------
	userTypes_6Uint16_t adcValuesDrv;
	DRVADC_getAdcValues(&adcValuesDrv);

	// Convert Adc Values to Midi Values
	//-----------------------------------
	midiValues.val1 = maxMidiMsgValue - map(adcValuesDrv.val1, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
	midiValues.val2 = maxMidiMsgValue - map(adcValuesDrv.val2, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
	midiValues.val3 = maxMidiMsgValue - map(adcValuesDrv.val3, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
	midiValues.val4 = maxMidiMsgValue - map(adcValuesDrv.val4, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
	midiValues.val5 = maxMidiMsgValue - map(adcValuesDrv.val5, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
	midiValues.val6 = maxMidiMsgValue - map(adcValuesDrv.val6, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);

	// Send Midi Values if different
	//-------------------------------
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x46, &(midiValues_old.val1), midiValues.val1);
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x47, &(midiValues_old.val2), midiValues.val2);
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x48, &(midiValues_old.val3), midiValues.val3);
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x49, &(midiValues_old.val4), midiValues.val4);
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x4A, &(midiValues_old.val5), midiValues.val5);
	sendCCIfDifferent(CMidiChannel::Channel_e::Ch01, 0x4B, &(midiValues_old.val6), midiValues.val6);
}

//------------------------------------------------------------------------------
/// \fn 		void pmgr_thread::sendCCIfDifferent(uint8_t channel, uint8_t cc, uint8_t *pOldData, uint8_t actualData)
/// \brief		Send Midi Values if different
//------------------------------------------------------------------------------
void pmgr_thread::sendCCIfDifferent(uint8_t channel, uint8_t cc, uint8_t *pOldData, uint8_t actualData)
{
	if(*pOldData != actualData){
		mdi_thread::getInstance().putMessage(channel, cc, actualData);
		*pOldData = actualData;
	}

}

//=============================================================================
//								Public Methods
//=============================================================================

//------------------------------------------------------------------------------
/// \fn 		pmgr_thread& pmgr_thread::getInstance()
/// \brief		Get the actual Instance
//------------------------------------------------------------------------------
pmgr_thread& pmgr_thread::getInstance()
{
    static pmgr_thread instance;
    return instance;
}

//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef pmgr_thread::init()
/// \brief		Initialisation du composant
//------------------------------------------------------------------------------
HAL_StatusTypeDef pmgr_thread::init()
{
	if (threadId == nullptr)
	{
		const osThreadAttr_t threadAttr = {
			.name = "pmgr",
			.stack_size = stackSize,
			.priority = priority,
		};

		// Création du thread
		threadId = osThreadNew(threadFunction, this, &threadAttr);
		if (threadId == nullptr) {
			return HAL_ERROR;
		}
	}

    return HAL_OK;
}

//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef pmgr_thread::exit()
/// \brief		Finalisation du composant
//------------------------------------------------------------------------------
HAL_StatusTypeDef pmgr_thread::exit()
{
	// Thread
	thread_stopRequested = true;
	while (threadId != nullptr) {
		osDelay(10); // Attente que le thread se termine proprement
	}

    return HAL_OK;
}


//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef pmgr_thread::setMode(FsmState state)
/// \brief		Set Mode
//------------------------------------------------------------------------------
void pmgr_thread::setMode(FsmState state)
{
	currentState = state;
}

