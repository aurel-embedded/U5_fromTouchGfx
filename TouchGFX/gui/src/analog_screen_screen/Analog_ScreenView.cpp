#include <gui/analog_screen_screen/Analog_ScreenView.hpp>
#include <Tools/Utilities.hpp>

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
	static drvAdc_values_t adc_old;

	drvAdc_values_t adc_actual = presenter->getAdcValues(potarDisplayRange);

	if(adc_old.potar1 != adc_actual.potar1){
		// Display Rotary Progress
		channel1_progress.setValue(potarDisplayRange - adc_actual.potar1 + 1);
		channel1_progress.invalidate();

		// Display Value
		Unicode::snprintf(channel1_valBuffer, CHANNEL1_VAL_SIZE, "%d", adc_actual.potar1);
		channel1_val.invalidate();

	    // Vérification des limites & conversion
		int value_loc = convertToMidi(adc_actual.potar1, 0, potarDisplayRange);

		// Send Midi
		presenter->handleChannelEvent(1, value_loc);

		// Store old Value
		adc_old.potar1 = adc_actual.potar1;
	}

}


int Analog_ScreenView::convertToMidi(int value, int minValue, int maxValue)
{
    // Clamp la valeur pour rester dans les limites
    value = clamp(value, minValue, maxValue);

    // Convertit en plage MIDI 0-127
    return (value - minValue) * 127 / (maxValue - minValue);
}
