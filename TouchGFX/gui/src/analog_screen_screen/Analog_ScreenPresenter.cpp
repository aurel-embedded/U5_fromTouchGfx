#include <gui/analog_screen_screen/Analog_ScreenView.hpp>
#include <gui/analog_screen_screen/Analog_ScreenPresenter.hpp>

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

drvAdc_values_t Analog_ScreenPresenter::getAdcValues(void)
{
	return model->getAdcValues();
}
