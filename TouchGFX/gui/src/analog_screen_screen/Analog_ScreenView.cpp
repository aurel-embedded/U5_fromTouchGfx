#include <gui/analog_screen_screen/Analog_ScreenView.hpp>

Analog_ScreenView::Analog_ScreenView()
{

}

void Analog_ScreenView::setupScreen()
{
    Analog_ScreenViewBase::setupScreen();
}

void Analog_ScreenView::tearDownScreen()
{
    Analog_ScreenViewBase::tearDownScreen();
}

void Analog_ScreenView::handleTickEvent()
{
	static int counter = 0;
	static drvAdc_values_t adc_old;
	counter++;

	drvAdc_values_t adc_actual = presenter->getAdcValues();

	if(	(adc_old.potar1 == adc_actual.potar1) &&
		(adc_old.potar2 == adc_actual.potar2) &&
		(adc_old.potar3 == adc_actual.potar3) &&
		(adc_old.potar4 == adc_actual.potar4) &&
		(adc_old.potar5 == adc_actual.potar5) &&
		(adc_old.potar6 == adc_actual.potar6)){
		return;
	}

	adc_old = adc_actual;

	// Display Potar
	if(counter % 2 == 0){
		channel1_progress.setValue(100 - adc_actual.potar1 + 1);
		channel1_progress.invalidate();
		channel2_progress.setValue(100 - adc_actual.potar2 + 1);
		channel2_progress.invalidate();
		channel3_progress.setValue(100 - adc_actual.potar3 + 1);
		channel3_progress.invalidate();
		channel4_progress.setValue(100 - adc_actual.potar4 + 1);
		channel4_progress.invalidate();
		channel5_progress.setValue(100 - adc_actual.potar5 + 1);
		channel5_progress.invalidate();
		channel6_progress.setValue(100 - adc_actual.potar6 + 1);
		channel6_progress.invalidate();
	}
}

