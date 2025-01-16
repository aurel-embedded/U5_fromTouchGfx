#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <MDI_midi/mdi_thread.hpp>
#include <UserInterfaces/drvAdc/drvAdc.h>
#include <Tools/Tools.h>
#include <UserInterfaces/PotarManager/pmgr_thread.hpp>

#ifndef SIMULATOR
#include <MDI_midi/CMidi.h>
#endif

Model::Model() : modelListener(0), activeViewId(ViewId::Unknown)
{
	pmgr_thread::getInstance().setMode(pmgr_thread::FsmState::Mode1);

}

void Model::tick()
{

}

void Model::sendMidiOn(int note)
{
	// TODO: Passer par mdi_thread::getInstance().putMessage
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
	// TODO: Passer par mdi_thread::getInstance().putMessage
#ifndef SIMULATOR
	if(note == 1)
		CMidi::sendNoteOff(CMidiChannel::Ch02, CMidiNote::C3, 127);
	else if (note == 2)
		CMidi::sendNoteOff(CMidiChannel::Ch01, CMidiNote::C4, 127);
	else
		CMidi::sendNoteOff(CMidiChannel::Ch03, CMidiNote::C0, 127);
#endif

}

void Model::sendControlChange(CMidiChannel::Channel_e channel, uint8_t cc, uint8_t data)
{
#ifndef SIMULATOR
	mdi_thread::getInstance().putMessage(channel, cc, data);
#endif

}

userTypes_6Uint16_t Model::getAdcValuesMapped(uint16_t maxMappedVal)
{
#ifndef SIMULATOR
	userTypes_6Uint16_t adcValuesDrv, mapValues;
	DRVADC_getAdcValues(&adcValuesDrv);
	mapValues.val1 = map(adcValuesDrv.val1, 0, 3300, 0, maxMappedVal);
	mapValues.val2 = map(adcValuesDrv.val2, 0, 3300, 0, maxMappedVal);
	mapValues.val3 = map(adcValuesDrv.val3, 0, 3300, 0, maxMappedVal);
	mapValues.val4 = map(adcValuesDrv.val4, 0, 3300, 0, maxMappedVal);
	mapValues.val5 = map(adcValuesDrv.val5, 0, 3300, 0, maxMappedVal);
	mapValues.val6 = map(adcValuesDrv.val6, 0, 3300, 0, maxMappedVal);
	return mapValues;

#endif

}

void Model::setPmgrMode(pmgr_thread::FsmState state)
{
#ifndef SIMULATOR
	pmgr_thread::getInstance().setMode(state);

#endif

}
