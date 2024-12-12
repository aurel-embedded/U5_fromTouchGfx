#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <MDI_midi/CMidi.h>

Model::Model() : modelListener(0)
{

}

void Model::tick()
{

}

void Model::sendMidiOn(int note)
{
	if(note == 1)
		CMidi::sendNoteOn(CMidiChannel::Ch02, CMidiNote::C3, 127);
	else if (note == 2)
		CMidi::sendNoteOn(CMidiChannel::Ch01, CMidiNote::C4, 127);
	else
		CMidi::sendNoteOn(CMidiChannel::Ch03, CMidiNote::C0, 127);

}

void Model::sendMidiOff(int note)
{
	if(note == 1)
		CMidi::sendNoteOff(CMidiChannel::Ch02, CMidiNote::C3, 127);
	else if (note == 2)
		CMidi::sendNoteOff(CMidiChannel::Ch01, CMidiNote::C4, 127);
	else
		CMidi::sendNoteOff(CMidiChannel::Ch03, CMidiNote::C0, 127);

}
