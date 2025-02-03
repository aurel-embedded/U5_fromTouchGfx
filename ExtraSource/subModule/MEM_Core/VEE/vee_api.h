/*
 * vee_api.h
 *
 *  Created on: 18 avr. 2024
 *      Author: apajadon
 */

#ifndef MEM_CORE_VEE_API_H_
#define MEM_CORE_VEE_API_H_
#include <eeprom_emul_types.h>
#include <stdint.h>

typedef enum {
  vee_error__OK = 0U,
  vee_error__dataNotFound, 	  // Variable is not found
  vee_error__Error,
  vee_error__drvFlashInitError,
  vee_error__drvFlashDeInitError,
  vee_error__veeInitError,
  vee_error__VeeNotFound,
  vee_error__writeError,
  vee_error__readError,
  vee_error__formatError,
  vee_error__cleanupError,

} vee_error_e;



extern vee_error_e 	VEE_init(void);
extern vee_error_e 	VEE_exit(void);
extern vee_error_e 	VEE_write(uint16_t VirtAddress, uint32_t data);
extern vee_error_e 	VEE_read(uint16_t VirtAddress, uint32_t* data);
extern vee_error_e 	VEE_format(EE_Erase_type EraseType);
extern vee_error_e 	VEE_cleanUp();

extern uint32_t 	VEE_get_variablesQty();
extern uint32_t 	VEE_get_activePage();
extern uint32_t 	VEE_get_activePageAddress();
extern uint32_t 	VEE_get_nbMaxElementsByPage();
extern uint32_t 	VEE_get_pagesQty();
extern uint32_t 	VEE_get_nbMaxWrittenElements();
extern uint32_t 	VEE_get_startPage();
extern uint32_t 	VEE_get_endPage();
extern uint32_t 	VEE_get_startEepromAddress();
extern uint32_t 	VEE_get_endEepromAddress();
extern uint8_t 		VEE_get_pageState(uint16_t page);
extern char* 		VEE_get_pageState_ToString(uint16_t page);

#endif /* MEM_CORE_VEE_API_H_ */
