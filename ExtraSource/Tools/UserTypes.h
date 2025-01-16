/*
 * UserTypes.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: apajadon
 */

#ifndef TOOLS_USERTYPES_H_
#define TOOLS_USERTYPES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint16_t 		val1;
	uint16_t 		val2;
	uint16_t 		val3;
	uint16_t 		val4;
	uint16_t 		val5;
	uint16_t 		val6;
}userTypes_6Uint16_t;

typedef struct{
	uint8_t 		val1;
	uint8_t 		val2;
	uint8_t 		val3;
	uint8_t 		val4;
	uint8_t 		val5;
	uint8_t 		val6;
}userTypes_6Uint8_t;

typedef struct{
	float 		val1;
	float 		val2;
	float 		val3;
	float 		val4;
	float 		val5;
	float 		val6;
}userTypes_6Float_t;

#ifdef __cplusplus
}
#endif




#endif /* TOOLS_USERTYPES_H_ */
