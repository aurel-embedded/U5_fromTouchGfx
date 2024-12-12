/*
 * CMidiNote.cpp
 *
 *  Created on: 28 avr. 2023
 *      Author: apajadon
 */

#include <iostream>
#include "CMidiNote.h"


// Special behavior for ++CMidiNote
CMidiNote& CMidiNote::operator++() {
	using IntType = typename std::underlying_type<Note_e>::type;
	this->val = static_cast<Note_e>( static_cast<IntType>(this->val) + 1 );
	if ( this->val == Note_e::NOTES_QTY )
		this->val = Note_e::C0;
	return *this;
}

// Special behavior for CMidiNote++
CMidiNote CMidiNote::operator++(int a) {
	using IntType = typename std::underlying_type<Note_e>::type;
	this->val = static_cast<Note_e>( static_cast<IntType>(this->val) + 1 );
	if ( this->val == Note_e::NOTES_QTY )
		this->val = Note_e::C0;
	return *this;
}

// Special behavior for --CMidiNote
CMidiNote& CMidiNote::operator--() {
	using IntType = typename std::underlying_type<Note_e>::type;
	if ( this->val == Note_e::C0 )
		this->val = Note_e::B5;
	else{
		this->val = static_cast<Note_e>( static_cast<IntType>(this->val) - 1 );
	}
	return *this;
}

// Special behavior for CMidiNote--
CMidiNote CMidiNote::operator--(int a) {
	using IntType = typename std::underlying_type<Note_e>::type;
	if ( this->val == Note_e::C0 )
		this->val = Note_e::B5;
	else{
		this->val = static_cast<Note_e>( static_cast<IntType>(this->val) - 1 );
	}
	return *this;
}

