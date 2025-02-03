/*
 * thw.h
 *
 *  Created on: 20 janv. 2023
 *      Author: apajadon
 */

#ifndef THW_THW_H_
#define THW_THW_H_


// Main
//------
extern void thw_main_setActive(void);

// HardFault
//-----------
extern void thw_HardFault_setActive(void);

// Driver
//--------
extern void thw_drv_uartThw_setActive(void);
extern void thw_drv_adc_setActive(void);
//extern void thw_drv_adcFdcEvt_setActive(void);
extern void thw_drv_vee_setActive(void);
extern void thw_drv_vee2_setActive(void);
//extern void thw_drv_veeF_setActive(void);



#endif /* THW_THW_H_ */
