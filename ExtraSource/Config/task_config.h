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
#define TSK_CFG__PRIO__TSK_SYSMGR 			(osPriority_t)osPriorityNormal			// System Manager
#define TSK_CFG__PRIO__TSK_THW 				(osPriority_t)osPriorityNormal			// Test Hardware - Main task
#define TSK_CFG__PRIO__TSK_THW_RFS 			(osPriority_t)osPriorityNormal			// Test Hardware - Refresh Task
#define TSK_CFG__PRIO__TSK_REG 				(osPriority_t)osPriorityHigh			// Regulation
#define TSK_CFG__PRIO__TSK_REG_CTA			(osPriority_t)osPriorityNormal			// Regulation - CTA
#define TSK_CFG__PRIO__TSK_FDC_TEMP			(osPriority_t)osPriorityNormal			// FdCan - TEMP
#define TSK_CFG__PRIO__TSK_FDC1_RX 			(osPriority_t)osPriorityNormal			// FdCan - Rx
#define TSK_CFG__PRIO__TSK_FDC1_TX 			(osPriority_t)osPriorityNormal			// FdCan - Tx
#define TSK_CFG__PRIO__TSK_FDC1_PRS 		(osPriority_t)osPriorityAboveNormal		// FdCan - Parser
#define TSK_CFG__PRIO__TSK_FDC1_ERR_MNG 	(osPriority_t)osPriorityNormal			// FdCan - Error Manager
#define TSK_CFG__PRIO__TSK_BAL 				(osPriority_t)osPriorityNormal			// Buttons And Leds
#define TSK_CFG__PRIO__TSK_FAN 				(osPriority_t)osPriorityNormal			// Component FAN
#define TSK_CFG__PRIO__TSK_EVT				(osPriority_t)osPriorityNormal			// EVT- Event Manager
#define TSK_CFG__PRIO__TSK_MONITOR	 		(osPriority_t)osPriorityNormal			// MONITOR - System Monitor
#define TSK_CFG__PRIO__TSK_WATCHDOG	 		(osPriority_t)osPriorityNormal			// WATCHDOG
#define TSK_CFG__PRIO__TSK_MEM_VEE 			(osPriority_t)osPriorityNormal			// MEM VEE
#define TSK_CFG__PRIO__TSK_CPT	 			(osPriority_t)osPriorityNormal			// CPT (Counter component)
#define TSK_CFG__PRIO__TSK_SYSMGR_FUNCMODE 	(osPriority_t)osPriorityNormal			// System Management functional modes

// Stack Size
#define TSK_CFG__STACK__TSK_SYSMGR			(128 * 32)  // System Manager
#define TSK_CFG__STACK__TSK_THW 			(128 * 32)  // Test Hardware - Main task
#define TSK_CFG__STACK__TSK_THW_RFS 		(128 * 16)  // Test Hardware - Refresh Task
#define TSK_CFG__STACK__TSK_REG 			(128 * 16)  // Regulation
#define TSK_CFG__STACK__TSK_REG_CTA			(128 * 16)  // Regulation
#define TSK_CFG__STACK__TSK_FDC_TEMP		(128 * 16)  // FDC Temp
#define TSK_CFG__STACK__TSK_FDC1_RX 		(128 * 16)  // FdCan - Rx
#define TSK_CFG__STACK__TSK_FDC1_TX 		(128 * 16)  // FdCan - Tx
#define TSK_CFG__STACK__TSK_FDC1_PRS 		(128 * 16)  // FdCan - Parser
#define TSK_CFG__STACK__TSK_FDC1_ERR_MNG 	(128 * 16)  // FdCan - Error Manager
#define TSK_CFG__STACK__TSK_BAL 			(128 * 16)  // Buttons And Leds
#define TSK_CFG__STACK__TSK_FAN 			(128 * 16)  // Component FAN
#define TSK_CFG__STACK__TSK_EVT				(128 * 16)  // EVT - Event Manager
#define TSK_CFG__STACK__TSK_MONITOR		 	(128 * 16)  // MONITOR - System Monitor
#define TSK_CFG__STACK__TSK_WATCHDOG		(128 * 16)  // WATCHDOG
#define TSK_CFG__STACK__TSK_MEM_VEE 		(128 * 16)  // MEM VEE
#define TSK_CFG__STACK__TSK_CPT		 		(128 * 16)  // CPT (Counter component)
#define TSK_CFG__STACK__TSK_SYSMGR_FUNCMODE	(128 * 16)  // System Management Functional mode task


#endif /* INC_TASK_CONFIG_H_ */
