/*
 * cmp.h
 *
 *  Created on: 1 févr. 2023
 *      Author: apajadon
 */

#ifndef CMP_H_
#define CMP_H_
#include <stdbool.h>
#include <stdint.h>

// Component Finite State Machine
//--------------------------------
typedef enum{
	cmp_fsm_undef = 0,		// Starting
	cmp_fsm_init,			// Initializing
	cmp_fsm_running,		// Running
	cmp_fsm_deinit,			// Deinit
	cmp_fsm_failure,		// Failure
}cmp_fsm_t;


// Component Status
//--------------------------------
typedef enum{
	cmp_mode_notDefined = 0,	// Mode non défini
	cmp_mode_nominal,			// Mode Nominal
	cmp_mode_degraded,			// Mode Dégradé
	cmp_mode_failure,			// Mode Failure
}cmp_mode_t;
extern char * cmpModeToString(cmp_mode_t mode);



typedef struct{
	bool 			isTaskRunning;		// Is Thread Task running
	cmp_mode_t		cmpMode;			// Component Mode
	cmp_fsm_t 		fsm;				// Finite State Machine for The Component
	uint8_t 		errNumber;
}cmp_status_t;


#endif /* CMP_H_ */
