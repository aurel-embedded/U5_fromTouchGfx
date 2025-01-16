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
extern void thw_drv_setActive(void);
extern void thw_drv_uartThw_setActive(void);
extern void thw_drv_can_setActive(void);
extern void thw_drv_fan_setActive(void);
extern void thw_drv_evp_setActive(void);
extern void thw_drv_evs_setActive(void);
extern void thw_drv_adc_setActive(void);
extern void thw_drv_adcFdcEvt_setActive(void);
extern void thw_drv_flash_setActive(void);
extern void thw_drv_vee_setActive(void);
extern void thw_drv_veeF_setActive(void);
extern void thw_drv_cem_setActive(void);
extern void thw_drv_wdg_setActive(void);
extern void thw_drv_hotSwap_setActive(void);

// Component
//-----------
extern void thw_cmp_setActive(void);
extern void thw_cmp_fdc_setActive(void);
extern void thw_cmp_fdc_norm_setActive(void);
extern void thw_cmp_fdc_Hw_setActive(void);
extern void thw_cmp_fdc_bridge_setActive(void);
extern void thw_cmp_fdc_rxManagement_setActive(void);
extern void thw_cmp_reg_setActive(void);
extern void thw_cmp_all_setActive(void);
extern void thw_cmp_sysMgr_setActive(void);
extern void thw_cmp_evt_setActive(void);
extern void thw_cmp_fan_setActive(void);


#endif /* THW_THW_H_ */
