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
};

#endif // ANALOG_SCREENVIEW_HPP
