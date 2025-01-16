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


HAL_StatusTypeDef CMidi::waitForMidiIdle(uint32_t timeout_ms)
{
    uint32_t timeout = osKernelGetTickCount() + timeout_ms;
    while (USBD_MIDI_GetState(&hUsbDeviceHS) != MIDI_IDLE) {
        if (osKernelGetTickCount() > timeout) {
            return HAL_TIMEOUT;
        }
        osDelay(1);
    }
    return HAL_OK;
}


HAL_StatusTypeDef CMidi::sendNoteOn(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel)
{
	return sendNoteOn(pMidiCfg->getChannel(), note, vel);

}

HAL_StatusTypeDef CMidi::sendNoteOn(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel)
{
    if (waitForMidiIdle(100) != HAL_OK) {
        return HAL_TIMEOUT;
    }

    uint8_t msg[4] = {0x09, static_cast<uint8_t>(0x90 + (channel & 0x0F)), note, vel};
    return (USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) == USBD_OK) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef CMidi::sendNoteOff(const CMidiCfg *pMidiCfg, CMidiNote::Note_e note, uint8_t vel)
{
	return sendNoteOff(pMidiCfg->getChannel(), note, vel);
}

HAL_StatusTypeDef CMidi::sendNoteOff(CMidiChannel::Channel_e channel, CMidiNote::Note_e note, uint8_t vel)
{
    if (waitForMidiIdle(100) != HAL_OK) {
        return HAL_TIMEOUT;
    }

    uint8_t msg[4] = {0x08, static_cast<uint8_t>(0x80 + (channel & 0x0F)), note, vel};
    return (USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) == USBD_OK) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef CMidi::sendControlChange(const CMidiCfg *pMidiCfg, uint8_t data)
{
		return sendControlChange(pMidiCfg->getChannel(), pMidiCfg->getCcVal(), data);
}

HAL_StatusTypeDef CMidi::sendControlChange(uint8_t channel, uint8_t cc, uint8_t data)
{
	if (data > 127 || cc > 127) {
	    return HAL_ERROR;
	}

    if (waitForMidiIdle(100) != HAL_OK) {
        return HAL_TIMEOUT;
    }

    uint8_t msg[4] = {0x0B, static_cast<uint8_t>(0xB0 + (channel & 0x0F)), static_cast<uint8_t>(cc), data};
    return (USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) == USBD_OK) ? HAL_OK : HAL_ERROR;
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
    if (waitForMidiIdle(100) != HAL_OK) {
        return HAL_TIMEOUT;
    }

    uint8_t msg[4] = {0x0F, 0xF8, 0x00, 0x00};
    return (USBD_MIDI_SendReport(&hUsbDeviceHS, msg, 4) == USBD_OK) ? HAL_OK : HAL_ERROR;
}


