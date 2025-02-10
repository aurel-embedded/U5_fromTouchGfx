/*
 * RSD_resetSource.h
 *
 *  Created on: 2 févr. 2024
 *      Author: apajadon
 */

#ifndef DRVCPU_RSD_RESETSOURCE_H_
#define DRVCPU_RSD_RESETSOURCE_H_




typedef enum{
	rsd_resetSource__NoReset = 0x00,
	rsd_resetSource__Watchdog,
	rsd_resetSource__IndependentWatchdog,
	rsd_resetSource__LowPower,
	rsd_resetSource__Pin,
	rsd_resetSource__Software,
	rsd_resetSource__BOR,
	rsd_resetSource__CpuDomain,
}rsd_resetSource_e;



#endif /* DRVCPU_RSD_RESETSOURCE_H_ */
