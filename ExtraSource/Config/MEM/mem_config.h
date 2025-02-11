/*
 * mem_config.h
 *
 *  Created on: Feb 10, 2025
 *      Author: apajadon
 */

#ifndef CONFIG_MEM_MEM_CONFIG_H_
#define CONFIG_MEM_MEM_CONFIG_H_
#include <MEM_Core/mem_common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	mem_virtualAddress_ID_potar1_min,
	mem_virtualAddress_ID_potar1_max,
	mem_virtualAddress_ID_potar2_min,
	mem_virtualAddress_ID_potar2_max,
	mem_virtualAddress_ID_potar3_min,
	mem_virtualAddress_ID_potar3_max,
	mem_virtualAddress_ID_potar4_min,
	mem_virtualAddress_ID_potar4_max,
	mem_virtualAddress_ID_potar5_min,
	mem_virtualAddress_ID_potar5_max,
	mem_virtualAddress_ID_potar6_min,
	mem_virtualAddress_ID_potar6_max,
}mem_virtualAddress_ID_e;


#ifdef __cplusplus
}
#endif

#endif /* CONFIG_MEM_MEM_CONFIG_H_ */
