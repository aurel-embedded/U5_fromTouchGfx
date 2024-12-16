#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::midi1_btn_clicked()
{
	presenter->midiNote(1, midi1_btn.getPressedState());
}

void Screen1View::midi2_btn_clicked()
{
	presenter->midiNote(2, midi2_btn.getPressedState());
}
