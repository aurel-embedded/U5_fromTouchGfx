/*
 * pmgr_thread.cpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 *
 *      Design Pattern: Singleton
 *
 */
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
    	// Get ADC Value

    	// Manage Component State
    	switch (self->currentState) {
			case FsmState::Idle:
				break;

			case FsmState::Mode1:
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


