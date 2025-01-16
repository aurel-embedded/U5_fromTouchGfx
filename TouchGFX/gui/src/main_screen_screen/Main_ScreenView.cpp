#include <custom/ScreenManager.hpp>
#include <gui/main_screen_screen/Main_ScreenView.hpp>

Main_ScreenView::Main_ScreenView()
{
}

void Main_ScreenView::setupScreen()
{
    Main_ScreenViewBase::setupScreen();
	ScreenManager::getInstance().setCurrentScreen(this);
}

void Main_ScreenView::tearDownScreen()
{
    Main_ScreenViewBase::tearDownScreen();
}

void Main_ScreenView::midi1_btn_clicked()
{
	presenter->midiNote(1, midi1_btn.getPressedState());
}

void Main_ScreenView::midi2_btn_clicked()
{
	presenter->midiNote(2, midi2_btn.getPressedState());
}

