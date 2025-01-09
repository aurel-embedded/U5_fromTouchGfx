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
#include <UserInterfaces/PotarManager/pmgr_thread.hpp>



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
    while (!self->thread_stopRequested)
    {

    	// Manage Component State
    	switch (self->currentState) {
			case FsmState::Idle:
				break;

			case FsmState::Mode1:
				// Get ADC Values
				//----------------
				drvAdc_values_t adcValuesDrv;
				DRVADC_getAdcValues(&adcValuesDrv);

				// Convert Adc Values to Midi Values
				//-----------------------------------
				adcValuesDrv.potar1 = map(adcValuesDrv.potar1, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
				adcValuesDrv.potar2 = map(adcValuesDrv.potar2, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
				adcValuesDrv.potar3 = map(adcValuesDrv.potar3, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
				adcValuesDrv.potar4 = map(adcValuesDrv.potar4, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
				adcValuesDrv.potar5 = map(adcValuesDrv.potar5, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);
				adcValuesDrv.potar6 = map(adcValuesDrv.potar6, minVoltageValue, maxVoltageValue, minMidiMsgValue, maxMidiMsgValue);

				// Send Midi Values
				//------------------
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 70, adcValuesDrv.potar1);
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 71, adcValuesDrv.potar2);
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 72, adcValuesDrv.potar3);
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 73, adcValuesDrv.potar4);
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 74, adcValuesDrv.potar5);
				mdi_thread::getInstance().putMessage(CMidiChannel::Channel_e::Ch01, 75, adcValuesDrv.potar6);

				break;

			case FsmState::Error:
				break;

			default:
				break;
		}

    	osDelay(taskDelay);
    }
}


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


