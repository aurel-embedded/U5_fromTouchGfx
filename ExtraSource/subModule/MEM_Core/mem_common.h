/*
 * mem_common.h
 *
 *  Created on: 17 août 2023
 *      Author: apajadon
 */

#ifndef CMP_MEM_MEM_COMMON_H_
#define CMP_MEM_MEM_COMMON_H_


#include <cmsis_os2.h>
#include <eeprom_emul_conf.h>
#include <MEM_Core/mem_api.h>




//-----------------------------------------------------------------------------
// Config
//-----------------------------------------------------------------------------
typedef struct{
	uint16_t virtualAddress;
	uint64_t defaultValue;
}mem_config_pair_t;

extern mem_config_pair_t 	mem_config_tab[];
extern uint32_t 			mem_config_tabSize;

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------
extern mem_data_pair_t 	mem_data_tab[];
extern uint32_t 		mem_data_tabSize;


//-----------------------------------------------------------------------------
// INTERNAL DATA
//-----------------------------------------------------------------------------
typedef struct{
	cmp_status_t 	cmpStatus;
    mem_vee_info_t  info;
	uint8_t mem_vee_error_loading_to_ram;
} mem_vee_internalData_t;
extern mem_vee_internalData_t mem_vee_internalData;

// Handle
extern osMessageQueueId_t mem_vee_ct_mq_id;
extern osMemoryPoolId_t mem_vee_ct_memPool_id;

// Message QTY
#define MEM_VEE_CT_MQ_QTY 32
#define MEM_VEE_CT_MEM_POOL_QTY 32


// Messages ID for Tower Control
typedef enum{
	mem_vee_write_id = 0x00,
	mem_vee_format_id,
	mem_vee_cleanUp_id,
}mem_vee_cta_id_t;

// Message Type
typedef struct {
	mem_vee_cta_id_t 	ctaId;			// ActionId
	mem_data_pair_t		actualValue;
}mem_vee_ct_mq_item_t;

#define MEM_VEE_MTX_WAIT_TIME    osWaitForever


//-----------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-----------------------------------------------------------------------------
extern void 		mem_data_razValues(void);
extern mem_error_e 	mem_format(void);
extern mem_error_e 	mem_cleanUp(void);
extern mem_error_e 	mem_write(uint16_t VirtAddress, uint64_t data);
extern mem_error_e 	mem_read(uint16_t VirtAddress, uint64_t* data);
extern mem_error_e 	mem_loadRamWithVee(void);


#endif /* CMP_MEM_MEM_COMMON_H_ */
