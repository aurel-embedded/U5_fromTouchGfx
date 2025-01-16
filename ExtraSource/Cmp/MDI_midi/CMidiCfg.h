/*
 * CMidiCfg.h
 *
 *  Created on: 20 avr. 2023
 *      Author: apajadon
 */

#ifndef SRC_MDI_MIDI_CMIDICFG_H_
#define SRC_MDI_MIDI_CMIDICFG_H_
#include <MDI_midi/CMidiChannel.h>
#include <stdint.h>

class CMidiCfg
{

public:

	// Midi Mode
	typedef enum{
		Note = 0,
		CC,
	}mode;

	// Control Command
	typedef enum{
		bankSelect = 0x00,
		modulationWheel,
		breathController,

		footController = 0x04,
		portamentoTime,
		dataEntryMsb,
		channelVolume,
		balance,

		pan = 0x0A,
		expressionController,
		effectControl1,
		effectControl2,

		start = 102,
		stop = 103,
		Metro = 104,

		AllSoundsOff = 120,
		AllNotesOff = 123,
		AllNotesOff_OmniModeOff = 124,
		AllNotesOff_OmniModeOn = 125,
		AllNotesOff_MonoModeOn = 126,
		AllNotesOff_PolyModeOn = 127,

		none = 0xff,
	}cc;

public:
	class enteringModeCmd{
	public:
		CMidiChannel::Channel_e	m_channel;	// Midi Channel
		cc						m_cc;		// CC
		uint8_t					m_val;		// Value
		enteringModeCmd(CMidiChannel::Channel_e _channel, cc _cc, uint8_t _val): m_channel(_channel), m_cc(_cc), m_val(_val){};
	};


private:
	CMidiChannel::Channel_e	m_channel;	// Midi Channel
	mode 					m_mode;		// Midi Note (CC or Note)
	cc						m_ccVal;		// CC Value (if CC mode)

public:
	CMidiCfg(CMidiChannel::Channel_e channel, mode mode, cc ccVal)
		: m_channel(channel), m_mode(mode), m_ccVal(ccVal){};
	CMidiCfg() : m_channel(), m_mode(), m_ccVal(){};	// TODO: Change CMidiCfg()
	virtual ~CMidiCfg() {};

	cc getCcVal() const {
		return m_ccVal;
	};

	CMidiChannel::Channel_e getChannel() const {
		return m_channel;
	};

	mode getMode() const {
		return m_mode;
	};

};

#endif /* SRC_MDI_MIDI_CMIDICFG_H_ */
