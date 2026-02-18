/*
 * CMidiNote.h
 *
 *  Created on: 28 avr. 2023
 *      Author: apajadon
 */

#ifndef SRC_MDI_MIDI_CMIDINOTE_H_
#define SRC_MDI_MIDI_CMIDINOTE_H_

class CMidiNote {
public:
	typedef enum{
		C0 = 24, Cd0, D0, Dd0, E0, F0, Fd0, G0, Gd0, A0, Ad0, B0,
		C1, Cd1, D1, Dd1, E1, F1, Fd1, G1, Gd1, A1, Ad1, B1,
		C2, Cd2, D2, Dd2, E2, F2, Fd2, G2, Gd2, A2, Ad2, B2,
		C3, Cd3, D3, Dd3, E3, F3, Fd3, G3, Gd3, A3, Ad3, B3,
		C4, Cd4, D4, Dd4, E4, F4, Fd4, G4, Gd4, A4, Ad4, B4,
		C5, Cd5, D5, Dd5, E5, F5, Fd5, G5, Gd5, A5, Ad5, B5,
		NOTES_QTY
	}Note_e;

public:
	Note_e 	val;	// Note Value
	uint8_t	vel;	// Note Velocity
	CMidiNote() : val(Note_e::C3), vel(127){};
	CMidiNote(Note_e _note) : val(_note), vel(127){};
	CMidiNote(Note_e _note, uint8_t _vel) : val(_note), vel(_vel){};
	virtual ~CMidiNote();

	CMidiNote& 	operator++();		// Prefix
	CMidiNote 	operator++(int);	// PostFix
	CMidiNote& 	operator--() ;		// Prefix
	CMidiNote 	operator--(int);	// PostFix

};

#endif /* SRC_MDI_MIDI_CMIDINOTE_H_ */
