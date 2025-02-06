/*
 * vee.c
 *
 *  Created on: 18 avr. 2024
 *      Author: apajadon
 */
#include <cmp.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul.h>
#include <MEM_Core/mem_api.h>
#include <stdint.h>
#include <stddef.h>
#include <MEM_Core/VEE/vee_api.h>

//-----------------------------------------------------------------------------
// INTERNAL DATA
//-----------------------------------------------------------------------------
typedef struct{
	cmp_mode_t 		cmpStatus;
	cmp_mode_t 		VeeStatus;
} vee_internalData_t;
vee_internalData_t vee_internalData = {
		.cmpStatus = cmp_mode_notDefined,
		.VeeStatus = cmp_mode_notDefined,
};


//=====================================================================================================================
//=====================================================================================================================
//													LOCAL FUNCTIONS
//=====================================================================================================================
//=====================================================================================================================


//=====================================================================================================================
//=====================================================================================================================
//													API FUNCTIONS
//=====================================================================================================================
//=====================================================================================================================


//------------------------------------------------------------------------------
/// \fn 		mem_vee_error_e VEE_init(void)
/// \brief		Initialize component (Os not Running)
//------------------------------------------------------------------------------
vee_error_e VEE_init(void)
{

	// Component Status
	vee_internalData.cmpStatus = cmp_mode_notDefined;
	HAL_FLASH_Unlock();

	// Init VEE
	EE_Status ret = EE_Init(EE_CONDITIONAL_ERASE);
//	EE_Status ret = EE_Init(EE_FORCED_ERASE);
	if(ret != EE_OK){
		EE_Format(EE_FORCED_ERASE);
		return vee_error__veeInitError;
	}
	vee_internalData.VeeStatus = cmp_mode_nominal;
	return vee_error__OK;
}

//------------------------------------------------------------------------------
/// \fn 		mem_vee_error_e VEE_exit(void)
/// \brief		Finalize component (Os not Running)
//------------------------------------------------------------------------------
vee_error_e VEE_exit(void)
{
	// Exit VEE
	vee_internalData.VeeStatus = cmp_mode_notDefined;

	HAL_FLASH_Lock();
	return vee_error__OK;
}


//--------------------------------------------------------------------------------------------------------
/// \fn 		vee_error_e VEE_write(uint16_t VirtAddress, uint32_t data, vee_number_e vee_number)
/// \brief		Write to VEE
//--------------------------------------------------------------------------------------------------------
vee_error_e VEE_write(uint16_t VirtAddress, uint32_t data)
{
	// Write to Vee
	EE_Status 	status = EE_WriteVariable32bits((uint16_t)VirtAddress, data);
	if(status == EE_CLEANUP_REQUIRED)
	{
		// Execute CleanUp if required
		status = EE_CleanUp();
		if(status == EE_ERROR_NOERASING_PAGE)
			status = EE_OK;
	}
	if(status == EE_OK)
	{
		return vee_error__OK;
	}
	else
	{
		return vee_error__writeError;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/// \fn 		vee_error_e VEE_read(uint16_t VirtAddress, uint32_t* data, vee_number_e vee_number)
/// \brief		Read Data from VEE
//---------------------------------------------------------------------------------------------------------------------
vee_error_e VEE_read(uint16_t VirtAddress, uint32_t* pData)
{
	// Read from Vee
	EE_Status 	status = EE_ReadVariable32bits((uint16_t)VirtAddress, pData);
	if(status == EE_CLEANUP_REQUIRED)
	{
		// Execute CleanUp if required
		status = EE_CleanUp();
		if(status == EE_ERROR_NOERASING_PAGE)
			status = EE_OK;
	}

	if(status == EE_OK) {
		return vee_error__OK;
	}
	else if (status == EE_NO_DATA) {
		return vee_error__dataNotFound;
	}
	else {
		return vee_error__readError;
	}
}


//------------------------------------------------------------------------------
/// \fn 		vee_error_e VEE_format(vee_number_e vee_number)
/// \brief		Format VEE
//------------------------------------------------------------------------------
vee_error_e VEE_format(EE_Erase_type EraseType)
{
	// Format Vee
	EE_Status 	status = EE_Format(EraseType);
	if(status == EE_OK)
	{
		return vee_error__OK;
	}
	else
	{
		return vee_error__formatError;
	}
}


//------------------------------------------------------------------------------
/// \fn 		vee_error_e VEE_cleanUp(vee_number_e vee_number)
/// \brief		Finalize component
//------------------------------------------------------------------------------
vee_error_e VEE_cleanUp()
{
	// Cleanup Vee
	EE_Status 	status = EE_CleanUp();
	if(status == EE_OK)
		return vee_error__OK;
	else if(status == EE_ERROR_NOERASING_PAGE)
		return vee_error__OK;
	else
		return vee_error__cleanupError;
}



//=====================================================================================================================
//=====================================================================================================================
//												API FUNCTIONS - VEE INFORMATION
//=====================================================================================================================
//=====================================================================================================================


//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_variablesQty()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_variablesQty()
{
	return EE_ex_get_variablesQty();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_activePage()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_activePage()
{
	return EE_ex_get_activePage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_activePageAddress()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_activePageAddress()
{
	return EE_ex_get_activePageAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_nbMaxElementsByPage()
/// \brief		Finalize component
//------------------------------------------------------------------------------
uint32_t VEE_get_nbMaxElementsByPage()
{
	return EE_ex_get_nbMaxElementsByPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_pagesQty()
//------------------------------------------------------------------------------
uint32_t VEE_get_pagesQty()
{
	return EE_ex_get_pagesQty();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_nbMaxWrittenElements()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_nbMaxWrittenElements()
{
	return EE_ex_get_nbMaxWrittenElements();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_startPage()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_startPage()
{
	return EE_ex_get_startPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_endPage()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_endPage()
{
	return EE_ex_get_endPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_startEepromAddress()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_startEepromAddress()
{
	return EE_ex_get_startEepromAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_endEepromAddress()
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_endEepromAddress()
{
	return EE_ex_get_endEepromAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_pageState(uint16_t page)
/// \brief
//------------------------------------------------------------------------------
uint8_t VEE_get_pageState(uint16_t page)
{
	return EE_ex_get_pageState(page);
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_pageState_ToString(uint16_t page)
/// \brief
//------------------------------------------------------------------------------
char* VEE_get_pageState_ToString(uint16_t page)
{
	return EE_ex_get_pageState_ToString(page);
}

