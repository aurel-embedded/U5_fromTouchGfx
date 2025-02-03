/*
 * mem_api.h
 *
 *  Created on: 21 mars 2024
 *      Author: apajadon
 */

#ifndef CMP_MEM_MEM_API_H_
#define CMP_MEM_MEM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <cmp.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul_types.h>
#include <MEM_Core/VEE/vee_api.h>


typedef enum {
	mem_error__OK = 0U,
	mem_error__writeError,
	mem_error__readError,
	mem_error__formatError,
	mem_error__cleanupError,
	mem_error__ramConfigNotFound,
	mem_error__noData,
	mem_error__invalidVirtualAddress,
	mem_error__invalidItem,
	mem_error__memoryPoolAllocation,
	mem_error__messageQueuePut,

	mem_error__init_veeInit,
	mem_error__init_creatingMutex,
	mem_error__init_creatingMessageQueue,
	mem_error__init_creatingMemoryPool,
	mem_error__init_creatingTask,

	mem_error__exit_veeDeInit,

} mem_error_e;


typedef struct{
	uint32_t 	variableCpt;

	EE_Status 	(*cleanUp)(void);
	EE_Status 	(*format)(EE_Erase_type EraseType);
	EE_Status 	(*writeVariable32bits)(uint16_t VirtAddress, uint32_t Data);
	EE_Status 	(*readVariable32bits)(uint16_t VirtAddress, uint32_t* pData);
	uint32_t 	(*get_variablesQty)(void);
	uint32_t 	(*get_activePage)(void);
	uint32_t 	(*get_activePageAddress)(void);
	uint32_t 	(*get_nbMaxElementsByPage)(void);
	uint32_t 	(*get_pagesQty)(void);
	uint32_t 	(*get_nbMaxWrittenElements)(void);
	uint32_t 	(*get_startPage)(void);
	uint32_t 	(*get_endPage)(void);
	uint32_t 	(*get_startEepromAddress)(void);
	uint32_t 	(*get_endEepromAddress)(void);
}mem_vee_confEmul_t;


//-----------------------------------------------------------------------------
// MEMORY SHARED
//-----------------------------------------------------------------------------
typedef struct {
	uint16_t 		VirtAddress;
	uint32_t 		data;
}mem_vee_data_t;


//-----------------------------------------------------------------------------
// Info structure to get error counters
//-----------------------------------------------------------------------------
typedef struct {
	uint32_t cpt_osMsgQueuePutError;
	uint32_t cpt_InvalidVirtualAddressError;

}mem_vee_info_t;


//------------------------------------------------------------------------------
// API
//------------------------------------------------------------------------------
extern mem_error_e 	MEM_init(void);
extern mem_error_e 	MEM_exit(void);

extern mem_error_e 	MEM_write(uint16_t id, const uint8_t *data,uint8_t size);
extern mem_error_e 	MEM_readFromVee(uint16_t id, uint8_t* data, uint8_t size);
extern mem_error_e 	MEM_readFromRam(uint16_t id, uint8_t* data, uint8_t size);

extern mem_error_e 	MEM_reset();
extern cmp_status_t MEM_getCmpStatus(void);



// Extra API

#ifdef __cplusplus
}
#endif

#endif /* CMP_MEM_MEM_API_H_ */
