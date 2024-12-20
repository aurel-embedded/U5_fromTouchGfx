#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <MDI_midi/mdi_thread.hpp>

#ifndef SIMULATOR
#include <MDI_midi/CMidi.h>
#endif

Model::Model() : modelListener(0)
{

}

void Model::tick()
{

}

void Model::sendMidiOn(int note)
{
#ifndef SIMULATOR
	if(note == 1)
		CMidi::sendNoteOn(CMidiChannel::Ch02, CMidiNote::C3, 127);
	else if (note == 2)
		CMidi::sendNoteOn(CMidiChannel::Ch01, CMidiNote::C4, 127);
	else
		CMidi::sendNoteOn(CMidiChannel::Ch03, CMidiNote::C0, 127);
#endif

}

void Model::sendMidiOff(int note)
{
#ifndef SIMULATOR
	if(note == 1)
		CMidi::sendNoteOff(CMidiChannel::Ch02, CMidiNote::C3, 127);
	else if (note == 2)
		CMidi::sendNoteOff(CMidiChannel::Ch01, CMidiNote::C4, 127);
	else
		CMidi::sendNoteOff(CMidiChannel::Ch03, CMidiNote::C0, 127);
#endif

}

void Model::sendControlChange(CMidiChannel::Channel_e channel, CMidiCfg::cc cc, uint8_t data)
{
#ifndef SIMULATOR
	mdi_thread::getInstance().putMessage(channel, cc, data);
#endif

}
