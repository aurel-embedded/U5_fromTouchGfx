/*
 * mem_vee.c
 *
 *  Created on: 18 août 2023
 *      Author: apajadon
 */

#include <MEM_Core/mem_common.h>
#include "string.h"



//-----------------------------------------------------------------------------
// VEE  MIRROR DATA IN RAM
//-----------------------------------------------------------------------------
mem_vee_pairValues_t tab_vee[NB_OF_VARIABLES];
uint32_t tab_vee_size = sizeof(tab_vee) / sizeof(mem_vee_pairValues_t);

//=====================================================================================================
//						 Functions ()
//=====================================================================================================

//------------------------------------------------------------------------------
/// \fn 		mem_vee_confEmul_t * mem_getRamConfigurationSizeInstance(void)
/// \brief
//------------------------------------------------------------------------------
static uint32_t mem_getRamConfigurationSizeInstance(void)
{
	return tab_vee_size;
}


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_vee_write(mem_vee_virtual_addr_t VirtAddress, uint32_t data)
/// \brief		write in vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_write(uint16_t VirtAddress, uint32_t data)
{
	vee_error_e 	err = vee_error__OK;

	//check valid Virtual Address
	if (VirtAddress > 0 && VirtAddress < 0xFFFE)
	{
		// Write to VEE
		err = VEE_write((uint16_t)VirtAddress, data);
		if(err != vee_error__OK)
			return mem_error__writeError;

		// Map To RAM
		mem_vee_pairValues_t 	*pPairValuesToUse = 	mem_getRamConfigurationInstance();

		if(pPairValuesToUse == NULL){
			return mem_error__ramConfigNotFound;
		}
		pPairValuesToUse[VirtAddress-1].virtualAddress = VirtAddress;
		pPairValuesToUse[VirtAddress-1].data = data;
	}
	else
	{
		mem_vee_internalData.info.cpt_InvalidVirtualAddressError++;
		return mem_error__invalidVirtualAddress;
	}



	return mem_error__OK;
}


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_readFromVee(mem_vee_virtual_addr_t VirtAddress, uint32_t* data)
/// \brief		read from vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_readFromVee(uint16_t VirtAddress, uint32_t* pData)
{
	vee_error_e err = vee_error__OK;

	//check valid Virtual Address
	if (VirtAddress > 0 && VirtAddress < 0xFFFE)
	{
		err = VEE_read((uint16_t)VirtAddress, pData);
		if(err != vee_error__OK) {
			if(err == vee_error__dataNotFound) {
				return mem_error__noData;
			}
			else {
				return mem_error__readError;
			}
		}
	}
	else
	{
		mem_vee_internalData.info.cpt_InvalidVirtualAddressError++;
		return mem_error__invalidVirtualAddress;
	}

	return mem_error__OK;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_readFromRam(mem_vee_virtual_addr_t VirtAddress, uint32_t* data)
/// \brief		read from Ram
//--------------------------------------------------------------------------------------------------
mem_error_e mem_readFromRam(uint16_t VirtAddress, uint32_t* data)
{
	//check valid Virtual Address
	if (VirtAddress > 0 && VirtAddress < 0xFFFE)
	{
		// Get Pair Config to use
		mem_vee_pairValues_t *pPairValuesToUse = mem_getRamConfigurationInstance();
		if(pPairValuesToUse == NULL){
			return mem_error__ramConfigNotFound;
		}

		// Check
		if(pPairValuesToUse[VirtAddress-1].virtualAddress != VirtAddress){
			return mem_error__noData;
		}

		// Assign data
		*data = pPairValuesToUse[VirtAddress-1].data;
	}
	else
	{
		mem_vee_internalData.info.cpt_InvalidVirtualAddressError++;
		return mem_error__invalidVirtualAddress;
	}



	return mem_error__OK;
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

	// Raz RAM
	mem_vee_pairValues_t 	*pPairValuesToUse = 	mem_getRamConfigurationInstance();
	uint32_t				pairValuesSizeToUse = 	mem_getRamConfigurationSizeInstance();

	if(pPairValuesToUse == NULL){
		return mem_error__ramConfigNotFound;
	}

	// Reset RAM
	memset(	pPairValuesToUse,
			0,
			pairValuesSizeToUse);

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


//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_loadDataToRam_item(void)
/// \brief		read from vee
//--------------------------------------------------------------------------------------------------
static mem_error_e mem_loadDataToRam_item(int addMax)
{
	mem_error_e errReturn = mem_error__OK;
	vee_error_e errRead = vee_error__OK;
	uint32_t varValue;

	// Get Pair Config to use
	mem_vee_pairValues_t *pPairValuesToUse = mem_getRamConfigurationInstance();
	if(pPairValuesToUse == NULL){
		return mem_error__ramConfigNotFound;
	}

	for(int i = 1; i < addMax; i++)
	{
		errRead = VEE_read(i, &varValue);
		if(errRead ==  vee_error__OK){
			pPairValuesToUse[i-1].data = varValue;
			// Assign Virtual Address
			pPairValuesToUse[i-1].virtualAddress = i;
		}
		else if(errRead !=  vee_error__dataNotFound) {
			errReturn = mem_error__readError;
		}
	}

	return errReturn;
}

//--------------------------------------------------------------------------------------------------
/// \fn 		EE_Status mem_loadDataFromVeeToRam(void)
/// \brief		read from vee
//--------------------------------------------------------------------------------------------------
mem_error_e mem_loadDataFromVeeToRam(void)
{
	mem_error_e errReturn = mem_error__OK;
	mem_error_e err = mem_error__OK;

	err = mem_loadDataToRam_item(NB_OF_VARIABLES+1);
	if(err != mem_error__OK)
		errReturn = err;


	return errReturn;
}

