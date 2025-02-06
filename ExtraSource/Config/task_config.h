/*
 * task_config.h
 *
 *  Created on: 3 janv. 2023
 *      Author: apajadon
 */

#ifndef INC_TASK_CONFIG_H_
#define INC_TASK_CONFIG_H_

#include <cmsis_os2.h>


// Task Priority
#define TSK_CFG__PRIO__TSK_THW 				(osPriority_t)osPriorityNormal			// Test Hardware - Main task
#define TSK_CFG__PRIO__TSK_THW_RFS 			(osPriority_t)osPriorityNormal			// Test Hardware - Refresh Task
#define TSK_CFG__PRIO__TSK_BAL 				(osPriority_t)osPriorityLow				// Buttons And Leds
#define TSK_CFG__PRIO__TSK_MDI			 	(osPriority_t)osPriorityNormal			// Midi
#define TSK_CFG__PRIO__TSK_PMGR			 	(osPriority_t)osPriorityNormal			// Potar Manager
#define TSK_CFG__PRIO__TSK_ADC			 	(osPriority_t)osPriorityNormal			// ADC
#define TSK_CFG__PRIO__TSK_MEM_VEE 			(osPriority_t)osPriorityNormal			// MEM VEE

// Stack Size
#define TSK_CFG__STACK__TSK_THW 			(128 * 32)  // Test Hardware - Main task
#define TSK_CFG__STACK__TSK_THW_RFS 		(128 * 16)  // Test Hardware - Refresh Task
#define TSK_CFG__STACK__TSK_BAL 			(128 * 16)  // Buttons And Leds
#define TSK_CFG__STACK__TSK_MIDI			(128 * 32)  // Midi
#define TSK_CFG__STACK__TSK_PMGR			(128 * 32)  // Potar Manager
#define TSK_CFG__STACK__TSK_ADC				(128 * 16)  // ADC
#define TSK_CFG__STACK__TSK_MEM_VEE 		(128 * 16)  // MEM VEE


#endif /* INC_TASK_CONFIG_H_ */
