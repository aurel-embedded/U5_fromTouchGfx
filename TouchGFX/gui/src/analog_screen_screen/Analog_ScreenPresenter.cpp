#include <gui/analog_screen_screen/Analog_ScreenView.hpp>
#include <gui/analog_screen_screen/Analog_ScreenPresenter.hpp>
#include <Tools/Utilities.hpp>

Analog_ScreenPresenter::Analog_ScreenPresenter(Analog_ScreenView& v)
    : view(v)
{

}

void Analog_ScreenPresenter::activate()
{

}

void Analog_ScreenPresenter::deactivate()
{

}

drvAdc_values_t Analog_ScreenPresenter::getAdcValues(uint16_t maxMappedVal)
{
	return model->getAdcValuesMapped(maxMappedVal);
}

void Analog_ScreenPresenter::handleChannelEvent(int channel, int value)
{
    // Envoi des messages MIDI
    model->sendControlChange(CMidiChannel::Channel_e::Ch01, CMidiCfg::cc::effectControl1, value);
}



