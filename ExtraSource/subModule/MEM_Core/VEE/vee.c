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
	cmp_mode_t 		Vee1_Status;
	cmp_mode_t 		Vee2_Status;
	cmp_mode_t 		Vee3_Status;
} vee_internalData_t;
vee_internalData_t vee_internalData = {
		.cmpStatus = cmp_mode_notDefined,
		.Vee1_Status = cmp_mode_notDefined,
		.Vee2_Status = cmp_mode_notDefined,
		.Vee3_Status = cmp_mode_notDefined,
};

//=====================================================================================================================
//=====================================================================================================================
//													VEE - FOPS CONFIGURATION
//=====================================================================================================================
//=====================================================================================================================

//VEE1
mem_vee_confEmul_t mem_vee_confItem_vee1 = {
		.variableCpt = 				0,
		.writeVariable32bits = 		EE_WriteVariable32bits,
		.readVariable32bits = 		EE_ReadVariable32bits,
		.cleanUp = 					EE_CleanUp,
		.format = 					EE_Format,
		.get_variablesQty = 		EE_ex_get_variablesQty,
		.get_activePage = 			EE_ex_get_activePage,
		.get_activePageAddress = 	EE_ex_get_activePageAddress,
		.get_nbMaxElementsByPage = 	EE_ex_get_nbMaxElementsByPage,
//		.get_pagesQty =				EE_ex_get_pagesQty,
		.get_nbMaxWrittenElements = EE_ex_get_nbMaxWrittenElements,
//		.get_startPage = 			EE_ex_get_startPage,
//		.get_endPage = 				EE_ex_get_endPage,
//		.get_startEepromAddress = 	EE_ex_get_startEepromAddress,
//		.get_endEepromAddress = 	EE_ex_get_endEepromAddress,
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
	vee_internalData.Vee1_Status = cmp_mode_nominal;
	return vee_error__OK;
}

//------------------------------------------------------------------------------
/// \fn 		mem_vee_error_e VEE_exit(void)
/// \brief		Finalize component (Os not Running)
//------------------------------------------------------------------------------
vee_error_e VEE_exit(void)
{
	// Exit VEE
	vee_internalData.Vee1_Status = cmp_mode_notDefined;

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
	EE_Status 	status = EE_Format(EE_FORCED_ERASE);
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
/// \fn 		uint32_t VEE_get_variablesQty(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_variablesQty()
{
	return EE_ex_get_variablesQty();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_activePage(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_activePage()
{
	return EE_ex_get_activePage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_activePageAddress(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_activePageAddress()
{
	return EE_ex_get_activePageAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_nbMaxElementsByPage(vee_number_e vee_number)
/// \brief		Finalize component
//------------------------------------------------------------------------------
uint32_t VEE_get_nbMaxElementsByPage()
{
	return EE_ex_get_nbMaxElementsByPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_pagesQty(vee_number_e vee_number)
//------------------------------------------------------------------------------
uint32_t VEE_get_pagesQty()
{
	return EE_ex_get_pagesQty();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_nbMaxWrittenElements(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_nbMaxWrittenElements()
{
	return EE_ex_get_nbMaxWrittenElements();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_startPage(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_startPage()
{
	return EE_ex_get_startPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_endPage(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_endPage()
{
	return EE_ex_get_endPage();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_startEepromAddress(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_startEepromAddress()
{
	return EE_ex_get_startEepromAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_endEepromAddress(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint32_t VEE_get_endEepromAddress()
{
	return EE_ex_get_endEepromAddress();
}

//------------------------------------------------------------------------------
/// \fn 		uint32_t VEE_get_pageState(vee_number_e vee_number)
/// \brief
//------------------------------------------------------------------------------
uint8_t VEE_get_pageState(uint16_t page)
{
	return EE_ex_get_pageState(page);
}

