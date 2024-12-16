/*
 * CMidiChannel.cpp
 *
 *  Created on: 27 avr. 2023
 *      Author: apajadon
 */
#include <iostream>
#include "CMidiChannel.h"

// Special behavior for ++CMidiChannel
CMidiChannel& CMidiChannel::operator++() {
	using IntType = typename std::underlying_type<Channel_e>::type;
	this->val = static_cast<Channel_e>( static_cast<IntType>(this->val) + 1 );
	if ( this->val == Channel_e::CHANNEL_QTY )
		this->val = Channel_e::Ch01;
	return *this;
}

// Special behavior for CMidiChannel++
CMidiChannel CMidiChannel::operator++(int a) {
	using IntType = typename std::underlying_type<Channel_e>::type;
	this->val = static_cast<Channel_e>( static_cast<IntType>(this->val) + 1 );
	if ( this->val == Channel_e::CHANNEL_QTY )
		this->val = Channel_e::Ch01;
	return *this;
}

// Special behavior for --CMidiChannel
CMidiChannel& CMidiChannel::operator--() {
	using IntType = typename std::underlying_type<Channel_e>::type;
	if ( this->val == Channel_e::Ch01 )
		this->val = Channel_e::Ch16;
	else{
		this->val = static_cast<Channel_e>( static_cast<IntType>(this->val) - 1 );
	}
	return *this;
}

// Special behavior for CMidiChannel--
CMidiChannel CMidiChannel::operator--(int a) {
	using IntType = typename std::underlying_type<Channel_e>::type;
	if ( this->val == Channel_e::Ch01 )
		this->val = Channel_e::Ch16;
	else{
		this->val = static_cast<Channel_e>( static_cast<IntType>(this->val) - 1 );
	}
	return *this;
}

