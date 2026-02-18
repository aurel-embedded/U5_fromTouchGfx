/*
 * vee_config.h
 *
 *  Created on: Feb 10, 2025
 *      Author: apajadon
 */

#ifndef CONFIG_MEM_VEE_CONFIG_H_
#define CONFIG_MEM_VEE_CONFIG_H_
#include <MEM_Core/mem_common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	vee_virtualAddress_ID_doNotUse_0 = 0,
	vee_virtualAddress_ID_potar1_minMax,
	vee_virtualAddress_ID_potar2_minMax,
	vee_virtualAddress_ID_potar3_minMax,
	vee_virtualAddress_ID_potar4_minMax,
	vee_virtualAddress_ID_potar5_minMax,
	vee_virtualAddress_ID_potar6_minMax,
	vee_virtualAddress_ID_doNotUse_FFFF = 0xFFFF,
}vee_virtualAddress_ID_e;


#ifdef __cplusplus
}
#endif

#endif /* CONFIG_MEM_VEE_CONFIG_H_ */
