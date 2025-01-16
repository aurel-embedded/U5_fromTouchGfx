/*
 * pmgr_thread.hpp
 *
 *  Created on: Dec 19, 2024
 *      Author: apajadon
 *
 *      Design Pattern: Singleton
 */

#ifndef PMGR_PMGR_THREAD_HPP_
#define PMGR_PMGR_THREAD_HPP_

#include <Config/task_config.h>
#include "cmsis_os2.h"
#include "main.h"
#include <atomic>

class pmgr_thread
{
public:
    static pmgr_thread& getInstance();

    HAL_StatusTypeDef init();
    HAL_StatusTypeDef exit();

    enum class FsmState {Idle, Mode1, Error};
    void setMode(FsmState state);

private:
    static constexpr uint8_t minMidiMsgValue = 0;
    static constexpr uint8_t maxMidiMsgValue = 127;
    static constexpr uint16_t minVoltageValue = 0;
    static constexpr uint16_t maxVoltageValue = 3300;

    // Constructeur privé
    pmgr_thread();
    ~pmgr_thread();

    // Interdire la copie et l’affectation
    pmgr_thread(const pmgr_thread&) = delete;
    pmgr_thread& operator=(const pmgr_thread&) = delete;

    // Attributs privés

    // Thread
    //----------
    std::atomic<bool> thread_stopRequested{false};
    static void threadFunction(void* argument); 						// Fonction statique pour le thread
    static void threadFunction_mode1(void* argument); 					// Fonction statique pour le thread
    static void sendCCIfDifferent(uint8_t channel, uint8_t cc, uint8_t *pOldData, uint8_t actualData);
    osThreadId_t threadId;                      						// ID du thread
    static constexpr uint32_t stackSize = TSK_CFG__STACK__TSK_PMGR; 	// Taille de la pile
    static constexpr osPriority_t priority = TSK_CFG__PRIO__TSK_PMGR; 	// Priorité
    static constexpr uint32_t taskDelay = 50;

    // Fsm
    //----------
    FsmState currentState = FsmState::Idle;

};

#endif /* PMGR_PMGR_THREAD_HPP_ */
