/*
 * ScreenManager.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: apajadon
 */

#ifndef GUI_INCLUDE_CUSTOM_SCREENMANAGER_HPP_
#define GUI_INCLUDE_CUSTOM_SCREENMANAGER_HPP_

#include <touchgfx/Screen.hpp>

class ScreenManager
{
public:
    static ScreenManager& getInstance()
    {
        static ScreenManager instance;
        return instance;
    }

    void setCurrentScreen(touchgfx::Screen* screen)
    {
        currentScreen = screen;
    }

    touchgfx::Screen* getCurrentScreen() const
    {
        return currentScreen;
    }

private:
    ScreenManager() : currentScreen(nullptr) {}
    touchgfx::Screen* currentScreen;
};


#endif /* GUI_INCLUDE_CUSTOM_SCREENMANAGER_HPP_ */
