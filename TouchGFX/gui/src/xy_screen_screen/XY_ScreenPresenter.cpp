#include <cmsis_os2.h>
#include <gui/xy_screen_screen/XY_ScreenView.hpp>
#include <gui/xy_screen_screen/XY_ScreenPresenter.hpp>
#include <MDI_midi/CMidiChannel.h>
#include <Tools/Utilities.hpp>

XY_ScreenPresenter::XY_ScreenPresenter(XY_ScreenView& v)
: view(v), touchPad_xMin(0), touchPad_xMax(320), touchPad_yMin(0), touchPad_yMax(240)
{

}

void XY_ScreenPresenter::activate()
{
	touchPad_xMin = view.getTouchPictureX();
	touchPad_xMax = touchPad_xMin + view.getTouchPictureWidth();
	touchPad_yMin = view.getTouchPictureY();
	touchPad_yMax = touchPad_yMin + view.getTouchPictureHeight();}

void XY_ScreenPresenter::deactivate()
{

}


void XY_ScreenPresenter::handleXYEvent(int XValue, int YValue)
{
    // Vérification des limites & conversion
    XValue = convertXYToMidi(XValue, touchPad_xMin, touchPad_xMax);
    YValue = convertXYToMidi(YValue, touchPad_yMin, touchPad_yMax);

    // Envoi des messages MIDI
    model->sendControlChange(CMidiChannel::Channel_e::Ch01, CMidiCfg::cc::effectControl1, XValue);
    model->sendControlChange(CMidiChannel::Channel_e::Ch02, CMidiCfg::cc::effectControl1, YValue);
}


int XY_ScreenPresenter::convertXYToMidi(int value, int minValue, int maxValue)
{
    // Clamp la valeur pour rester dans les limites
    value = clamp(value, minValue, maxValue);

    // Convertit en plage MIDI 0-127
    return (value - minValue) * 127 / (maxValue - minValue);
}
