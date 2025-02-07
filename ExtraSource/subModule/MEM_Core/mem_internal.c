/*
 * mem_vee.c
 *
 *  Created on: 18 août 2023
 *      Author: apajadon
 */

#include <MEM_Core/mem_common.h>
#include "string.h"



//-----------------------------------------------------------------------------
// VEE
//-----------------------------------------------------------------------------
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

mem_virtualAddressData_pair_t mem_values_tab[] = {
		{.virtualAddress = mem_virtualAddress_ID_potar1_min},
		{.virtualAddress = mem_virtualAddress_ID_potar1_max},
		{.virtualAddress = mem_virtualAddress_ID_potar2_min},
		{.virtualAddress = mem_virtualAddress_ID_potar2_max},
		{.virtualAddress = mem_virtualAddress_ID_potar3_min},
		{.virtualAddress = mem_virtualAddress_ID_potar3_max},
		{.virtualAddress = mem_virtualAddress_ID_potar4_min},
		{.virtualAddress = mem_virtualAddress_ID_potar4_max},
		{.virtualAddress = mem_virtualAddress_ID_potar5_min},
		{.virtualAddress = mem_virtualAddress_ID_potar5_max},
		{.virtualAddress = mem_virtualAddress_ID_potar6_min},
		{.virtualAddress = mem_virtualAddress_ID_potar6_max},
};
uint32_t mem_values_tabSize = sizeof(mem_values_tab) / sizeof(mem_virtualAddressData_pair_t);

static mem_virtualAddressData_pair_t * mem_getPairFromVirtualAddress(mem_virtualAddress_ID_e id);
static void mem_razValues(void);

//=====================================================================================================
//						 		COMMON COMPONENT FUNCTIONS
//=====================================================================================================


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_vee_write(mem_vee_virtual_addr_t VirtAddress, uint32_t data)
/// \brief		write in vee & in Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_write(uint16_t VirtAddress, uint32_t data)
{
	vee_error_e 	err = vee_error__OK;

	//check valid Virtual Address
	if (VirtAddress == 0 || VirtAddress == 0xFFFF){
		return mem_error__invalidVirtualAddress;
	}

	// Write to VEE
	err = VEE_write((uint16_t)VirtAddress, data);
	if(err != vee_error__OK)
		return mem_error__writeError;

	// Write To RAM
	mem_virtualAddressData_pair_t 	*pPairValuesToUse = mem_getPairFromVirtualAddress(VirtAddress);
	if(pPairValuesToUse == NULL){
		return mem_error__ramConfigNotFound;
	}
	pPairValuesToUse->data = data;

	return mem_error__OK;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_read(mem_vee_virtual_addr_t VirtAddress, uint32_t* data)
/// \brief		read from Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_read(uint16_t VirtAddress, uint32_t* data)
{
	//check valid Virtual Address
	if (VirtAddress == 0 || VirtAddress == 0xFFFF){
		return mem_error__invalidVirtualAddress;
	}

	// Get Pair Config to use
	mem_virtualAddressData_pair_t 	*pPairValuesToUse = mem_getPairFromVirtualAddress(VirtAddress);
	if(pPairValuesToUse == NULL){
		return mem_error__ramConfigNotFound;
	}

	// Assign data
	*data = pPairValuesToUse->data;

	return mem_error__OK;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_vee_write(mem_vee_virtual_addr_t VirtAddress, uint32_t data)
/// \brief		write in vee & in Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_loadRamWithVee()
{
	uint32_t data;
	bool atLeastOneError = false;

	for (uint8_t idx = 0; idx < mem_values_tabSize; idx++) {
		if(mem_values_tab[idx].virtualAddress != 0){
			// Read From VEE
			vee_error_e err = VEE_read((uint16_t)mem_values_tab[idx].virtualAddress, &data);
			if(err == vee_error__OK){
				// Copy to Ram
				mem_values_tab[idx].data = data;
			}else{
				atLeastOneError = true;
			}
				return mem_error__writeError;
		}
	}

	if(atLeastOneError == true)
		return mem_error__writeError;
	else
		return mem_error__readError;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_format(void)
/// \brief		read from vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_format(void)
{
	vee_error_e err = vee_error__OK;

	// Format VEE
	err = VEE_format(EE_FORCED_ERASE);
	if(err != vee_error__OK)
		return mem_error__formatError;

	// Reset RAM
	mem_razValues();

	return mem_error__OK;
}


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_cleanUp(void)
/// \brief		read from vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_cleanUp(void)
{
	vee_error_e err = VEE_cleanUp();
	if(err != vee_error__OK)
		return mem_error__cleanupError;
	else
		return mem_error__OK;
}


//=====================================================================================================
//						 				LOCAL FUNCTIONS
//=====================================================================================================

//--------------------------------------------------------------------------------------------------
/// \fn 		mem_virtualAddressData_pair_t * mem_getPairFromVirtualAddress(mem_virtualAddress_ID_e id)
/// \brief		get pair values from tab
//--------------------------------------------------------------------------------------------------
static mem_virtualAddressData_pair_t * mem_getPairFromVirtualAddress(mem_virtualAddress_ID_e id)
{
	for (uint8_t idx = 0; idx < mem_values_tabSize; idx++) {
		if(mem_values_tab[idx].virtualAddress == id){
			return &(mem_values_tab[idx]);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		static void mem_razValues(void)
/// \brief		Raz All Values (don't erase virtualAddress)
//--------------------------------------------------------------------------------------------------
static void mem_razValues(void)
{
	for (uint8_t idx = 0; idx < mem_values_tabSize; idx++) {
		if(mem_values_tab[idx].virtualAddress != 0){
			mem_values_tab[idx].data = 0;
		}
	}
}
