/*
 * CMidi.h
 *
 *  Created on: 20 avr. 2023
 *      Author: apajadon
 */

#ifndef SRC_MDI_MIDI_CMIDI_H_
#define SRC_MDI_MIDI_CMIDI_H_
#include <stm32u5xx.h>
#include <stm32u5xx_hal_def.h>
#include "CMidiCfg.h"
#include "CMidiNote.h"

class CMidi {
public:
	CMidi();
	virtual ~CMidi();

	static HAL_StatusTypeDef waitForMidiIdle(uint32_t timeout_ms);
	static HAL_StatusTypeDef sendNoteOn(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel);
	static HAL_StatusTypeDef sendNoteOn(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel);
	static HAL_StatusTypeDef sendNoteOff(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel);
	static HAL_StatusTypeDef sendNoteOff(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel);
	static HAL_StatusTypeDef sendControlChange(const CMidiCfg *pMidiCfg, uint8_t data);
	static HAL_StatusTypeDef sendControlChange(uint8_t channel, uint8_t cc, uint8_t data);
	static HAL_StatusTypeDef sendPlay(void);
	static HAL_StatusTypeDef sendStop(void);
	static HAL_StatusTypeDef sendMetro(void);
	static HAL_StatusTypeDef sendAllNotesOff(void);
	static HAL_StatusTypeDef sendTempo(void);
};


#endif /* SRC_MDI_MIDI_CMIDI_H_ */
