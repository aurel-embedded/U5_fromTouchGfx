/*
 * mem_config.c
 *
 *  Created on: Feb 10, 2025
 *      Author: apajadon
 */
#include "mem_config.h"
#include <MEM_Core/mem_common.h>


#define MEM_MIN_POTAR_DEFAULT 0
#define MEM_MAX_POTAR_DEFAULT 127

mem_config_pair_t mem_config_tab[] = {
		{.virtualAddress = mem_virtualAddress_ID_potar1_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar1_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar2_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar2_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar3_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar3_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar4_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar4_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar5_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar5_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar6_min, .defaultValue = MEM_MIN_POTAR_DEFAULT},
		{.virtualAddress = mem_virtualAddress_ID_potar6_max, .defaultValue = MEM_MAX_POTAR_DEFAULT},
};
uint32_t mem_config_tabSize = sizeof(mem_config_tab) / sizeof(mem_config_pair_t);
