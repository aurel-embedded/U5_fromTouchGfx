#include <custom/ScreenManager.hpp>
#include <gui/analog_screen_screen/Analog_ScreenView.hpp>
#include <Tools/Utilities.hpp>

Analog_ScreenView::Analog_ScreenView()
{

}

void Analog_ScreenView::setupScreen()
{
	ScreenManager::getInstance().setCurrentScreen(this);
    Analog_ScreenViewBase::setupScreen();

}

void Analog_ScreenView::tearDownScreen()
{
    Analog_ScreenViewBase::tearDownScreen();
}

void Analog_ScreenView::handleTickEvent()
{
	static userTypes_6Uint16_t adc_old;

	userTypes_6Uint16_t adc_actual = presenter->getAdcValues(potarDisplayRange);

	handleTickEvent_Item(adc_actual.val1, &(adc_old.val1), &channel1_progress, &channel1_val, channel1_valBuffer, CHANNEL1_VAL_SIZE);
	handleTickEvent_Item(adc_actual.val2, &(adc_old.val2), &channel2_progress, &channel2_val, channel2_valBuffer, CHANNEL2_VAL_SIZE);
	handleTickEvent_Item(adc_actual.val3, &(adc_old.val3), &channel3_progress, &channel3_val, channel3_valBuffer, CHANNEL3_VAL_SIZE);
	handleTickEvent_Item(adc_actual.val4, &(adc_old.val4), &channel4_progress, &channel4_val, channel4_valBuffer, CHANNEL4_VAL_SIZE);
	handleTickEvent_Item(adc_actual.val5, &(adc_old.val5), &channel5_progress, &channel5_val, channel5_valBuffer, CHANNEL5_VAL_SIZE);
	handleTickEvent_Item(adc_actual.val6, &(adc_old.val6), &channel6_progress, &channel6_val, channel6_valBuffer, CHANNEL6_VAL_SIZE);

}

void Analog_ScreenView::handleTickEvent_Item(	uint16_t actualValue, uint16_t *pOldValue,
												touchgfx::CircleProgress *pProgress,
												touchgfx::TextAreaWithOneWildcard *pText,
												touchgfx::Unicode::UnicodeChar *pBuf,
												uint16_t bufSize)
{
	if(*pOldValue != actualValue){
		// Display Rotary Progress
		pProgress->setValue(potarDisplayRange - actualValue + 1);
		pProgress->invalidate();

		// Display Value
		Unicode::snprintf(pBuf, bufSize, "%d", actualValue);
		pText->invalidate();

		// Store old Value
		*pOldValue = actualValue;
	}
}


