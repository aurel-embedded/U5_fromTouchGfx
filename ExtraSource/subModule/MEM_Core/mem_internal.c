/*
 * mem_vee.c
 *
 *  Created on: 18 août 2023
 *      Author: apajadon
 */

#include <MEM/mem_config.h>
#include <MEM_Core/mem_common.h>
#include "string.h"



//-----------------------------------------------------------------------------
// VEE
//-----------------------------------------------------------------------------


static mem_config_pair_t * 	mem_config_getPair(mem_virtualAddress_ID_e id);
static mem_data_pair_t * 	mem_data_getPair(mem_virtualAddress_ID_e id);



//=====================================================================================================
//						 		COMMON COMPONENT FUNCTIONS
//=====================================================================================================


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_vee_write(mem_vee_virtual_addr_t VirtAddress, uint32_t data)
/// \brief		write in vee & in Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_write(uint16_t VirtAddress, uint64_t data)
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
	mem_data_pair_t 	*pDataPairToUse = mem_data_getPair(VirtAddress);
	if(pDataPairToUse == NULL){
		return mem_error__ramConfigNotFound;
	}
	pDataPairToUse->data = data;

	return mem_error__OK;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_read(mem_vee_virtual_addr_t VirtAddress, uint32_t* data)
/// \brief		read from Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_read(uint16_t VirtAddress, uint64_t* data)
{
	//check valid Virtual Address
	if (VirtAddress == 0 || VirtAddress == 0xFFFF){
		return mem_error__invalidVirtualAddress;
	}

	// Get Pair Config to use
	mem_data_pair_t 	*pDataPairToUse = mem_data_getPair(VirtAddress);
	if(pDataPairToUse == NULL){
		return mem_error__ramConfigNotFound;
	}

	// Assign data
	*data = pDataPairToUse->data;

	return mem_error__OK;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_vee_write(mem_vee_virtual_addr_t VirtAddress, uint32_t data)
/// \brief		Read Vee & write to Ram
//					if virtualId not found, write default Value to Ram & Vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_loadRamWithVee()
{
	uint64_t data;
	bool atLeastOneError = false;
	vee_error_e vee_err = vee_error__OK;

	// Browsing Mem config tab
	for (uint8_t idx = 0; idx < mem_config_tabSize; idx++)
	{
		if(mem_config_tab[idx].virtualAddress != 0)
		{
			// Read From VEE
			vee_err = VEE_read((uint16_t)mem_config_tab[idx].virtualAddress, &data);

			// Read - Ok
			if(vee_err == vee_error__OK)
			{
				// Copy to Ram
				mem_data_tab[idx].data = data;
			}
			// Read - Data Not Found
			else if(vee_err == vee_error__dataNotFound)
			{
				// Write default value to Vee
				vee_err = VEE_write((uint16_t)mem_config_tab[idx].virtualAddress, mem_config_tab[idx].defaultValue);

				// Copy to Ram
				mem_data_tab[idx].data = data;
			}
			// Read - Error
			else
			{
				atLeastOneError = true;
			}
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
	mem_data_razValues();

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
static mem_config_pair_t * mem_config_getPair(mem_virtualAddress_ID_e id)
{
	for (uint8_t idx = 0; idx < mem_config_tabSize; idx++) {
		if(mem_config_tab[idx].virtualAddress == id){
			return &(mem_config_tab[idx]);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		mem_data_pair_t * mem_data_getPair(mem_virtualAddress_ID_e id)
/// \brief		get pair values from tab
//--------------------------------------------------------------------------------------------------
static mem_data_pair_t * mem_data_getPair(mem_virtualAddress_ID_e id)
{
	for (uint8_t idx = 0; idx < mem_data_tabSize; idx++) {
		if(mem_data_tab[idx].virtualAddress == id){
			return &(mem_data_tab[idx]);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		static void mem_data_razValues(void)
/// \brief		Raz All Values from data tab (RAM) (don't erase virtualAddress)
//--------------------------------------------------------------------------------------------------
void mem_data_razValues(void)
{
	for (uint8_t idx = 0; idx < mem_data_tabSize; idx++) {
		if(mem_data_tab[idx].virtualAddress != 0){
			mem_data_tab[idx].data = 0;
		}
	}
}
