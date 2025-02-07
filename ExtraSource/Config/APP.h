/*
 * APP.h
 *
 *  Created on: 13 janv. 2023
 *      Author: apajadon
 */

#ifndef INC_APP_H_
#define INC_APP_H_
#include <stdint.h>
#include "main.h"

static const uint8_t 	APP_SoftwareReference[]	= "AMI";
static const uint8_t 	APP_SoftwareVersion[]	= "0.2.0";


//=============================================================================
// 								Alimentation
//=============================================================================

// BOR
//------
#define DRVCPU_BOR_ENABLE 	1
#define DRVCPU_BOR_LEVEL	OB_BOR_LEVEL_2 /*!< Reset level threshold is around 2.2V */



//=============================================================================

#endif /* INC_APP_H_ */
