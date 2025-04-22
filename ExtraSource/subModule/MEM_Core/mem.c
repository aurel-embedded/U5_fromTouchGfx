/*
 * mem.c
 *
 *  Created on: 17 août 2023
 *      Author: apajadon
 */

#include <main.h>
#include <cmsis_os2.h>
#include <DBG/dbg_debugPrintf.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul_types.h>
#include <MEM_Core/mem_api.h>
#include <MEM_Core/mem_common.h>
#include <Config/MEM/mem_config.h>
#include <Config/MEM/vee_config.h>
#include <task_config.h>
#include "string.h"

// DEBUG MEASUREMENT ON GPIO
// In order to see if we can really stop the mem task when powering off
#define MEM_DEBUG_MEASUREMENT_ON_GPIO 1

//-----------------------------------------------------------------------------
// BACKUP SRAM
//-----------------------------------------------------------------------------
#define MEM_BKPSRAM_ADDRESS  	((uint32_t*)0x38800000)  // Adresse de la Backup SRAM
#define MEM_BKPSRAM_TAG1_VEE1	0xDEAD0EE1
#define MEM_BKPSRAM_TAG1_VEE2	0xDEAD0EE2
#define MEM_BKPSRAM_TAG1_VEE3	0xDEAD0EE3

//-----------------------------------------------------------------------------
// CONTROL TOWER
//-----------------------------------------------------------------------------
// Attribut
osMessageQueueAttr_t mem_vee_ct_mq_attr = {
		.name = "mem_vee_ct_mq",
};

// Attribut
osMemoryPoolAttr_t mem_vee_ct_memPool_attr = {
		.name = "mem_vee_ct_memPool",
};



// Handle
osMessageQueueId_t mem_vee_ct_mq_id;
osMemoryPoolId_t mem_vee_ct_memPool_id;


//-----------------------------------------------------------------------------
// INTERNAL DATA
//-----------------------------------------------------------------------------
mem_vee_internalData_t mem_vee_internalData = {
		.info = {
				.cpt_osMsgQueuePutError = 0,
		},
		.mem_vee_error_loading_to_ram = 0,
};



//-----------------------------------------------------------------------------
// CONFIG RAM
//-----------------------------------------------------------------------------
struct{
	mem_potar_cfg_st potar1;
	mem_potar_cfg_st potar2;
	mem_potar_cfg_st potar3;
	mem_potar_cfg_st potar4;
	mem_potar_cfg_st potar5;
	mem_potar_cfg_st potar6;
}mem_config;

uint32_t 			mem_data_tabSize = sizeof(mem_data_tab) / sizeof(mem_data_pair_t);
osMutexId_t  		mem_data_mtx_id;
const osMutexAttr_t mem_data_mtx_attr = {
		"mem_data_mtx",                             // human readable mutex name
		osMutexRecursive | osMutexPrioInherit,  	// attr_bits
		NULL,                                    	// memory for control block
		0U                                       	// size for control block
};


//-----------------------------------------------------------------------------
// Potar Mapping
//-----------------------------------------------------------------------------
typedef struct{
	mem_potar_ID_e				Id;
	vee_virtualAddress_ID_e 	vee_virtualAddress;
	mem_potar_cfg_st 			*pRam_cfgItem;
}mem_potar_mappingItem_st;

mem_potar_mappingItem_st mem_potar_mappingItemList[] = {
		// Mapping: Id potar,	Vee ID,	Ram Object
		{mem_potar1, vee_virtualAddress_ID_potar1_minMax, &(mem_config.potar1)},
		{mem_potar2, vee_virtualAddress_ID_potar2_minMax, &(mem_config.potar2)},
		{mem_potar3, vee_virtualAddress_ID_potar3_minMax, &(mem_config.potar3)},
		{mem_potar4, vee_virtualAddress_ID_potar4_minMax, &(mem_config.potar4)},
		{mem_potar5, vee_virtualAddress_ID_potar5_minMax, &(mem_config.potar5)},
		{mem_potar6, vee_virtualAddress_ID_potar6_minMax, &(mem_config.potar6)},
};
uint8_t mem_potar_mappingItemList_Size = sizeof(mem_potar_mappingItemList) / sizeof(mem_potar_mappingItem_st);

static mem_potar_mappingItem_st * mem_potar_mappingItem_findItem(mem_potar_ID_e potarId);

//-----------------------------------------------------------------------------
// THREAD
//-----------------------------------------------------------------------------
osThreadId_t mem_vee_tsk_id;
const osThreadAttr_t mem_vee_tsk_attr = {
		.name = "mem_vee_tsk",
		.stack_size = TSK_CFG__STACK__TSK_MEM_VEE,
		.priority = TSK_CFG__PRIO__TSK_MEM_VEE,
};
static void mem_vee_tsk_fn(void *arg);


static void mem_BackupSRAM_RetrieveFaultAddress(void);


//=============================================================================
//						Thread Function
//=============================================================================
static void mem_vee_tsk_fn(void *arg)
{
	mem_vee_ct_mq_item_t 	*msg;
	osStatus_t 	status;

	//init RAM values
	mem_data_razValues();

	//Loading already stored value if any to RAM
	if((mem_vee_internalData.cmpStatus.errNumber = mem_loadRamWithVee()) != mem_error__OK)
	{
		DBG_printf("MEM: Init...ERROR (mem_loadRamWithVee)\r\n");
		return;
	}
	// Component Status
	mem_vee_internalData.cmpStatus.isTaskRunning = true;
	mem_vee_internalData.cmpStatus.cmpMode = cmp_mode_nominal;

	while(1)
	{
		// Get message from messageQueue
		status = osMessageQueueGet(mem_vee_ct_mq_id, &msg, NULL, osWaitForever);   // wait for message
		if (status == osOK)
		{
			// Manage Request Action
			switch(msg->ctaId){
			case mem_vee_write_id:
				mem_write(msg->actualValue.virtualAddress,  msg->actualValue.data);
				// Deallocate the memory used by the message.
				osMemoryPoolFree(mem_vee_ct_memPool_id, msg);
				break;

			case mem_vee_format_id:
				mem_format();
				// Deallocate the memory used by the message.
				osMemoryPoolFree(mem_vee_ct_memPool_id, msg);
				break;

			case mem_vee_cleanUp_id:
				mem_cleanUp();
				// Deallocate the memory used by the message.
				osMemoryPoolFree(mem_vee_ct_memPool_id, msg);
				break;


			default:
				break;
			}

		}
	}
	osThreadExit();

}



//------------------------------------------------------------------------------
/// \fn 		void mem_BackupSRAM_RetrieveFaultAddress(void)
/// \brief
//------------------------------------------------------------------------------
static void mem_BackupSRAM_RetrieveFaultAddress(void)
{
    //TODO: mem_BackupSRAM_RetrieveFaultAddress
}

//=============================================================================
//						Externals Functions (API)
//=============================================================================


//------------------------------------------------------------------------------
/// \fn 		mem_vee_error_e MEM_init(void)
/// \brief		Initialize component (Os not Running)
//------------------------------------------------------------------------------
mem_error_e MEM_init(void)
{
	DBG_printf("MEM: Init...\r\n");

	mem_BackupSRAM_RetrieveFaultAddress();

	// Component Status
	mem_vee_internalData.cmpStatus.isTaskRunning = false;
	mem_vee_internalData.cmpStatus.cmpMode = cmp_mode_notDefined;

	// Init Vee Component
	if(VEE_getComponentStatus() != cmp_mode_nominal){
		if(VEE_init() != vee_error__OK){
			DBG_printf("MEM: Init...ERROR (INIT VEE)\r\n");
			return mem_error__init_veeInit;
		}
	}

	// Create Mutex
	mem_data_mtx_id = osMutexNew(&mem_data_mtx_attr);
	if (mem_data_mtx_id == NULL)  {
		DBG_printf("MEM: Init...ERROR (Mutex)\r\n");
		return mem_error__init_creatingMutex;
	}

	// Creating Message Queue
	mem_vee_ct_mq_id = osMessageQueueNew(MEM_VEE_CT_MQ_QTY, sizeof(mem_vee_ct_mq_item_t *), &mem_vee_ct_mq_attr);
	if (mem_vee_ct_mq_id == NULL) {
		osMutexDelete(mem_data_mtx_id);
		DBG_printf("MEM: Init...ERROR (MessageQueue)\r\n");
		return mem_error__init_creatingMessageQueue;
	}

	// Creating Memory Pool
	mem_vee_ct_memPool_id = osMemoryPoolNew(MEM_VEE_CT_MEM_POOL_QTY, sizeof(mem_vee_ct_mq_item_t), &mem_vee_ct_memPool_attr);
	if (mem_vee_ct_memPool_id == NULL) {
		osMutexDelete(mem_data_mtx_id);
		osMessageQueueDelete(mem_vee_ct_mq_id);
		DBG_printf("MEM: Init...ERROR (MemoryPool)\r\n");
		return mem_error__init_creatingMemoryPool;
	}

	// Creating Task
	mem_vee_tsk_id = osThreadNew(mem_vee_tsk_fn, NULL, &mem_vee_tsk_attr);
	if(mem_vee_tsk_id == NULL){
		osMutexDelete(mem_data_mtx_id);
		osMessageQueueDelete(mem_vee_ct_mq_id);
		osMemoryPoolDelete(mem_vee_ct_memPool_id);
		DBG_printf("MEM: Init...ERROR (Task)\r\n");
		return mem_error__init_creatingTask;
	}

	DBG_printf("MEM: Init...OK\r\n");
	return mem_error__OK;
}



//------------------------------------------------------------------------------
/// \fn 		mem_vee_error_e MEM_exit(void)
/// \brief		Finalize component
//------------------------------------------------------------------------------
mem_error_e MEM_exit(void)
{
	mem_error_e err =  mem_error__OK;

	// Delete All Os object
	osThreadTerminate(mem_vee_tsk_id);
	osMutexDelete(mem_data_mtx_id);
	osMessageQueueDelete(mem_vee_ct_mq_id);
	osMemoryPoolDelete(mem_vee_ct_memPool_id);

	//clean up VEE
	err = mem_cleanUp();

	// Exit Vee
	if(VEE_exit() != vee_error__OK){
		err |= mem_error__exit_veeDeInit;
	}

	// Component Status
	mem_vee_internalData.cmpStatus.isTaskRunning = false;
	mem_vee_internalData.cmpStatus.cmpMode = cmp_mode_notDefined;

	return err;
}





//--------------------------------------------------------------------------------------------------------
/// \fn 		MEM_write(uint16_t id, const uint8_t *data,size_t size)
/// \brief		Write to VEE
//--------------------------------------------------------------------------------------------------------
mem_error_e MEM_write(uint16_t id, const uint8_t *data)
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_write
	return status;
}


//---------------------------------------------------------------------------------------------------------------------
/// \fn 		MEM_readFromVee(uint16_t id, mem_dataType_t dataType,uint8_t* data, size_t size)
/// \brief		Read Data from VEE
//---------------------------------------------------------------------------------------------------------------------
mem_error_e MEM_read(uint16_t id, uint8_t* data)
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_readFromVee
	return status;
}



//------------------------------------------------------------------------------
/// \fn 		MEM_reset(mem_dataType_t dataType)
/// \brief		reset ram and vee data to zero
//------------------------------------------------------------------------------
mem_error_e MEM_reset()
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_reset
	return status;
}

//------------------------------------------------------------------------------
/// \fn 		cmp_status_t MEM_getCmpStatus(void)
/// \brief		return cmpstatus info
//------------------------------------------------------------------------------
cmp_status_t MEM_getCmpStatus(void)
{
	return mem_vee_internalData.cmpStatus;
}


//------------------------------------------------------------------------------
/// \fn 		void MEM_activateBusError(void)
/// \brief		activate bus error It (BusFault_Handler)
//------------------------------------------------------------------------------
void MEM_BusError_Activate(void)
{
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
}


//------------------------------------------------------------------------------
/// \fn 		mem_error_e MEM_potarCfg_write(uint8_t potarId, uint16_t min, uint16_t max)
/// \brief		write to memory (RAM & VEE)
//------------------------------------------------------------------------------
mem_error_e MEM_potarCfg_write(mem_potar_ID_e potarId, mem_potar_cfg_st cfg)
{
	// Find Mapping Item
	mem_potar_mappingItem_st *pCfgToUse = mem_potar_mappingItem_findItem(potarId);
	if(pCfgToUse == NULL)
		return mem_error__noData;

	// TODO: Convert data

	// TODO: Write to Vee

	// Write to Ram
	memcpy(pCfgToUse->pRam_cfgItem, &cfg, sizeof(mem_potar_cfg_st));

	return mem_error__OK;
}

//------------------------------------------------------------------------------
/// \fn 		MEM_potarCfg_read(uint8_t potarId, mem_potar_cfg_st *pCfg)
/// \brief		read from memory (RAM)
//------------------------------------------------------------------------------
mem_error_e MEM_potarCfg_read(mem_potar_ID_e potarId, mem_potar_cfg_st *pCfg)
{
	// Find Mapping Item
	mem_potar_mappingItem_st *pCfgToUse = mem_potar_mappingItem_findItem(potarId);
	if(pCfgToUse == NULL)
		return mem_error__noData;

	// Read From Ram
	memcpy(pCfg, pCfgToUse->pRam_cfgItem, sizeof(mem_potar_cfg_st));

	return mem_error__OK;
}

//------------------------------------------------------------------------------
/// \fn 		mem_potar_configItem_st mem_potar_configItem_findItem(mem_potar_ID_e potarId)
/// \brief		find Item
//------------------------------------------------------------------------------
static mem_potar_mappingItem_st * mem_potar_mappingItem_findItem(mem_potar_ID_e potarId)
{
	if(potarId >= mem_potar_mappingItemList_Size)
		return NULL;

	for(uint8_t i = 0; i < mem_potar_mappingItemList_Size; i++){
		if(mem_potar_mappingItemList[i].Id == potarId)
			return &(mem_potar_mappingItemList[i]);
	}
	return NULL;
}
