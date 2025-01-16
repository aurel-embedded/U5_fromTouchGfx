/*
 * CMidiChannel.h
 *
 *  Created on: 27 avr. 2023
 *      Author: apajadon
 */

#ifndef MDI_MIDI_CCHANNEL_H_
#define MDI_MIDI_CCHANNEL_H_
#include <cstdint>

class CMidiChannel
{
public:
	typedef enum : uint8_t{
		Ch01, Ch02, Ch03, Ch04,
		Ch05, Ch06, Ch07, Ch08,
		Ch09, Ch10, Ch11, Ch12,
		Ch13, Ch14, Ch15, Ch16,
		CHANNEL_QTY
	}Channel_e;

public:
	Channel_e val;
	CMidiChannel() : val(Channel_e::Ch01){};
	CMidiChannel(Channel_e value) : val(value){};
	virtual ~CMidiChannel(){};
	CMidiChannel& 	operator++();		// Prefix
	CMidiChannel 	operator++(int);	// PostFix
	CMidiChannel& 	operator--() ;		// Prefix
	CMidiChannel 	operator--(int);	// PostFix
};

#endif /* MDI_MIDI_CCHANNEL_H_ */
