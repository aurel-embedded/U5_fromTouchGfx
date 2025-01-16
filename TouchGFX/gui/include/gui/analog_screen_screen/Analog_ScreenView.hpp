#ifndef ANALOG_SCREENVIEW_HPP
#define ANALOG_SCREENVIEW_HPP

#include <gui_generated/analog_screen_screen/Analog_ScreenViewBase.hpp>
#include <gui/analog_screen_screen/Analog_ScreenPresenter.hpp>

class Analog_ScreenView : public Analog_ScreenViewBase
{
public:
    Analog_ScreenView();
    virtual ~Analog_ScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();


protected:

private:
    const int potarDisplayRange = 320;

    void handleTickEvent_Item(	uint16_t actualValue, uint16_t *pOldValue,
    							touchgfx::CircleProgress *pProgress,
    							touchgfx::TextAreaWithOneWildcard *pText,
    							touchgfx::Unicode::UnicodeChar *pBuf,
    							uint16_t bufSize);
};

#endif // ANALOG_SCREENVIEW_HPP
