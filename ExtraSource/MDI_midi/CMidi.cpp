/*
 * CMidi.cpp
 *
 *  Created on: 20 avr. 2023
 *      Author: apajadon
 */

#include <cmsis_os2.h>
#include "CMidi.h"
#include <usbd_midi.h>
#include <vector>

using std::vector;

extern USBD_HandleTypeDef hUsbDeviceHS;

CMidi::CMidi()
{
	// TODO Auto-generated constructor stub

}

CMidi::~CMidi()
{
	// TODO Auto-generated destructor stub
}

HAL_StatusTypeDef CMidi::sendNoteOn(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel)
{
	return sendNoteOn(pMidiCfg->getChannel(), note, vel);

}

HAL_StatusTypeDef CMidi::sendNoteOn(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel)
{
	while(USBD_MIDI_GetState(&hUsbDeviceHS) != MIDI_IDLE){
		osDelay(1);
	}

	uint8_t msg[4];
	msg[0] = 0x09;						// Send to cable 0, note on
//	msg[1] = 0x90 + ((uint8_t)(channel) & 0x0F);	// note on (again!) high byte, channel low byte
	msg[1] = 0x90 + ((channel) & 0x0F);	// note on (again!) high byte, channel low byte
	msg[2] = note;						// note
	msg[3] = vel;						// velocity
	if(USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) != USBD_OK){
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef CMidi::sendNoteOff(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel)
{
	return sendNoteOff(pMidiCfg->getChannel(), note, vel);
}

HAL_StatusTypeDef CMidi::sendNoteOff(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel)
{
	while(USBD_MIDI_GetState(&hUsbDeviceHS) != MIDI_IDLE){
		osDelay(1);
	}

	uint8_t msg[4];
	msg[0] = 0x08;												// Send to cable 0, note off
	msg[1] = 0x80 + ((uint8_t)(channel) & 0x0F);	// note off (again!) high byte, channel low byte
	msg[2] = note;												// note
	msg[3] = vel;												// velocity
	if(USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) != USBD_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef CMidi::sendControlChange(const CMidiCfg *pMidiCfg, uint8_t data)
{
		return sendControlChange(pMidiCfg->getChannel(), pMidiCfg->getCcVal(), data);
}

HAL_StatusTypeDef CMidi::sendControlChange(CMidiChannel::Channel_e channel, CMidiCfg::cc cc, uint8_t data)
{
	while(USBD_MIDI_GetState(&hUsbDeviceHS) != MIDI_IDLE){
		osDelay(1);
	}

	uint8_t msg[4];
	msg[0] = 0x0B;												// Send to cable 0, control Change
	msg[1] = 0xB0 + ((uint8_t)(channel) & 0x0F);	// CC (again!) high byte, channel low byte
	msg[2] = (uint8_t)(cc);						// control Number
	msg[3] = data;												// data
	if(USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) != USBD_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}



HAL_StatusTypeDef CMidi::sendPlay(void)
{
	return sendControlChange(CMidiChannel::Channel_e::Ch01, CMidiCfg::cc::start, 70);
}

HAL_StatusTypeDef CMidi::sendStop(void)
{
	return sendControlChange(CMidiChannel::Channel_e::Ch01, CMidiCfg::cc::stop, 70);
}

HAL_StatusTypeDef CMidi::sendMetro(void)
{
	return sendControlChange(CMidiChannel::Channel_e::Ch01, CMidiCfg::cc::Metro, 70);
}

HAL_StatusTypeDef CMidi::sendAllNotesOff(void)
{
	CMidiChannel channel(CMidiChannel::Channel_e::Ch01);
	for(uint8_t ind = 0; ind < CMidiChannel::Channel_e::CHANNEL_QTY; ind++){
		sendControlChange(channel.val, CMidiCfg::cc::AllNotesOff, 70);
		channel++;
		osDelay(1);
	}
	return HAL_OK;
}

HAL_StatusTypeDef CMidi::sendTempo(void)
{
	while(USBD_MIDI_GetState(&hUsbDeviceHS) != MIDI_IDLE){
		osDelay(1);
	}

	uint8_t msg[4];
	msg[0] = 0x0F;		// Default Midi Cable + CIN.
	msg[1] = 0xF8;		// Tempo
	msg[2] = 0x00;		// unused
	msg[3] = 0x00;		// unused
	if(USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) != USBD_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}


