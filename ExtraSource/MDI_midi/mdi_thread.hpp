/*
 * mdi_thread.hpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 *
 *      Design Pattern: Singleton
 */

#ifndef MDI_MIDI_MDI_THREAD_HPP_
#define MDI_MIDI_MDI_THREAD_HPP_

#include <Config/task_config.h>
#include <MDI_midi/CMidiCfg.h>
#include <MDI_midi/CMidiChannel.h>
#include "cmsis_os2.h"
#include "main.h"

class mdi_thread
{
public:
	typedef struct {
	    CMidiChannel::Channel_e channel; // Canal MIDI
	    CMidiCfg::cc cc;                 // Contrôle MIDI
	    uint8_t data;                    // Données MIDI
	} MsgQueueMessage;

public:
    static mdi_thread& getInstance();

    HAL_StatusTypeDef init();
    HAL_StatusTypeDef exit();
    HAL_StatusTypeDef putMessage(CMidiChannel::Channel_e channel, CMidiCfg::cc cc, uint8_t data);

private:
    // Constructeur privé
    mdi_thread();
    ~mdi_thread();

    // Interdire la copie et l’affectation
    mdi_thread(const mdi_thread&) = delete;
    mdi_thread& operator=(const mdi_thread&) = delete;

    // Attributs privés

    // Thread
    static void threadFunction(void* argument); 						// Fonction statique pour le thread
    osThreadId_t threadId;                      						// ID du thread
    static constexpr uint32_t stackSize = TSK_CFG__STACK__TSK_MIDI; 	// Taille de la pile
    static constexpr osPriority_t priority = TSK_CFG__PRIO__TSK_MDI; 	// Priorité
    static constexpr uint32_t period = 500;    // Période du thread en ms

    // Message Queue
    osMessageQueueAttr_t mq_attr = {
    		.name = "mdi_mq",
    };
    static osMessageQueueId_t mq_id;
    static constexpr uint32_t mq_qty = 10;

};

#endif /* MDI_MIDI_MDI_THREAD_HPP_ */
