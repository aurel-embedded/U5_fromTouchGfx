/*
 * mdi_thread.cpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 *
 *      Design Pattern: Singleton
 *
 */

#include <MDI_midi/CMidi.h>
#include <MDI_midi/mdi_thread.hpp>


osMessageQueueId_t mdi_thread::mq_id = nullptr;

// Constructeur privé : initialise les membres
mdi_thread::mdi_thread() : threadId(nullptr) {}

// Destructeur privé : arrête le thread s'il est actif
mdi_thread::~mdi_thread()
{
    exit(); // Assurer la terminaison du thread
}

//=============================================================================
//									TASK
//=============================================================================
// Fonction du thread (statique)
void mdi_thread::threadFunction(void* argument)
{
	osStatus_t 	status;
	MsgQueueMessage 	msg;

	mdi_thread* self  = static_cast<mdi_thread*>(argument); // Conversion en instance de la classe
    while (true)
    {
		status = osMessageQueueGet(mq_id, &msg, NULL, osWaitForever);   // wait for message
		if (status == osOK)
		{
			CMidi::sendControlChange(msg.channel, msg.cc, msg.data);
		}
    }
}


//------------------------------------------------------------------------------
/// \fn 		mdi_thread& mdi_thread::getInstance()
/// \brief		Get the actual Instance
//------------------------------------------------------------------------------
mdi_thread& mdi_thread::getInstance()
{
    static mdi_thread instance;
    return instance;
}

//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef mdi_thread::init()
/// \brief		Initialisation du composant
//------------------------------------------------------------------------------
HAL_StatusTypeDef mdi_thread::init()
{
    const osThreadAttr_t threadAttr = {
        .name = "mdi",
        .stack_size = stackSize,
        .priority = priority,
    };

    // Création du thread
    threadId = osThreadNew(threadFunction, this, &threadAttr);
    if (threadId == nullptr) {
        return HAL_ERROR;
    }

	// Creating Message Queue
    mq_id = osMessageQueueNew(mq_qty, sizeof(MsgQueueMessage), &mq_attr);
	if (mq_id == nullptr) {
        osThreadTerminate(threadId); // Terminer le thread
		return HAL_ERROR;
	}

    return HAL_OK;
}

//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef mdi_thread::exit()
/// \brief		Finalisation du composant
//------------------------------------------------------------------------------
HAL_StatusTypeDef mdi_thread::exit()
{
	// Thread
    if (threadId != nullptr) {
        osThreadTerminate(threadId); // Terminer le thread
        threadId = nullptr;
    }

    // Message Queue
    if (mq_id != nullptr) {
    	osMessageQueueDelete(mq_id);
        threadId = nullptr;
    }

    return HAL_OK;
}


//------------------------------------------------------------------------------
/// \fn 		HAL_StatusTypeDef mdi_thread::putMessage(reg_ctaId_t msg)
/// \brief
//------------------------------------------------------------------------------
HAL_StatusTypeDef mdi_thread::putMessage(CMidiChannel::Channel_e channel, CMidiCfg::cc cc, uint8_t data)
{
	if(mq_id == NULL)
		return HAL_ERROR;
	MsgQueueMessage msg = {
			.channel = channel,
			.cc = cc,
			.data = data,
	};
	osStatus_t status = osMessageQueuePut(mq_id, &msg, 0U, 0U);

	// Error Code
	if( status!= osOK){
		return HAL_ERROR;
	}else{
		return HAL_OK;
	}
}
