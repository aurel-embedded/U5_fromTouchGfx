/*
 * mem.c
 *
 *  Created on: 17 août 2023
 *      Author: apajadon
 */

#include <main.h>
#include <cmsis_os2.h>
#include <MEM_Core/EEPROM_Emul/Core/eeprom_emul_types.h>
#include <MEM_Core/mem_api.h>
#include <MEM_Core/mem_common.h>
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
// MEMORY SHARED
//-----------------------------------------------------------------------------
mem_vee_memoryShared_t mem_vee_memoryShared = {
		.mtx_id = NULL,
		.data = {
				.data = 0,
				.VirtAddress = 0,
		},
};


//-----------------------------------------------------------------------------
// MUTEX - Memory shared
//-----------------------------------------------------------------------------
const osMutexAttr_t mem_vee_mtx_attr = {
		"mem_vee_mtx",                             	// human readable mutex name
		osMutexRecursive | osMutexPrioInherit,  	// attr_bits
		NULL,                                    	// memory for control block
		0U                                       	// size for control block
};




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
	memset(tab_vee,0,sizeof(tab_vee));

	//Loading already stored value if any to RAM
	if((mem_vee_internalData.cmpStatus.errNumber = mem_loadDataToRam()) != mem_error__OK)
	{
		mem_vee_internalData.mem_vee_error_loading_to_ram = 1;
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
				mem_write(msg->data.VirtAddress,  msg->data.data);
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
	if(VEE_init() != vee_error__OK){
		DBG_printf("MEM: Init...ERROR (INIT VEE)\r\n");
		return mem_error__init_veeInit;
	}


	// Create Mutex
	mem_vee_memoryShared.mtx_id = osMutexNew(&mem_vee_mtx_attr);
	if (mem_vee_memoryShared.mtx_id == NULL)  {
		osMutexDelete(mem_vee_memoryShared.mtx_id);
		DBG_printf("MEM: Init...ERROR (Mutex)\r\n");
		return mem_error__init_creatingMutex;
	}

	// Creating Message Queue
	mem_vee_ct_mq_id = osMessageQueueNew(MEM_VEE_CT_MQ_QTY, sizeof(mem_vee_ct_mq_item_t *), &mem_vee_ct_mq_attr);
	if (mem_vee_ct_mq_id == NULL) {
		osMessageQueueDelete(mem_vee_ct_mq_id);
		DBG_printf("MEM: Init...ERROR (MessageQueue)\r\n");
		return mem_error__init_creatingMessageQueue;
	}

	// Creating Memory Pool
	mem_vee_ct_memPool_id = osMemoryPoolNew(MEM_VEE_CT_MEM_POOL_QTY, sizeof(mem_vee_ct_mq_item_t), &mem_vee_ct_memPool_attr);
	if (mem_vee_ct_memPool_id == NULL) {
		osMemoryPoolDelete(mem_vee_ct_memPool_id);
		DBG_printf("MEM: Init...ERROR (MemoryPool)\r\n");
		return mem_error__init_creatingMemoryPool;
	}

	// Creating Task
	mem_vee_tsk_id = osThreadNew(mem_vee_tsk_fn, NULL, &mem_vee_tsk_attr);
	if(mem_vee_tsk_id == NULL){
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

	// Delete task
	osThreadTerminate(mem_vee_tsk_id);

	// Delete message Queue
	osMutexDelete(mem_vee_memoryShared.mtx_id);

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
mem_error_e MEM_write(uint16_t id, const uint8_t *data,uint8_t size)
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_write
	return status;
}


//---------------------------------------------------------------------------------------------------------------------
/// \fn 		MEM_readFromVee(uint16_t id, mem_dataType_t dataType,uint8_t* data, size_t size)
/// \brief		Read Data from VEE
//---------------------------------------------------------------------------------------------------------------------
mem_error_e MEM_readFromVee(uint16_t id, uint8_t* data, uint8_t size)
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_readFromVee
	return status;
}


//---------------------------------------------------------------------------------------------------------------------
/// \fn 		MEM_readFromRam(uint16_t id, mem_dataType_t dataType,uint8_t* data, size_t size)
/// \brief		Read Data from RAM
//---------------------------------------------------------------------------------------------------------------------
mem_error_e MEM_readFromRam(uint16_t id, uint8_t* data, uint8_t size)
{
	mem_error_e status = mem_error__OK;
	//TODO: MEM_readFromRam
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


