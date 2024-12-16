/*
 * CustomButton.hpp
 *
 *  Created on: Dec 12, 2024
 *      Author: apajadon
 */

#ifndef GUI_INCLUDE_GUI_COMMON_CUSTOMBUTTON_HPP_
#define GUI_INCLUDE_GUI_COMMON_CUSTOMBUTTON_HPP_

#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/Callback.hpp>

using touchgfx::GenericCallback;

class CustomButton : public touchgfx::Button {
public:
    CustomButton() : onPressCallback(nullptr), onReleaseCallback(nullptr) {}

    void setOnPressCallback(GenericCallback<>& callback)
    {
        onPressCallback = &callback;
    }

    void setOnReleaseCallback(GenericCallback<>& callback)
    {
        onReleaseCallback = &callback;
    }

    virtual void handleClickEvent(const touchgfx::ClickEvent& event) override
    {
        Button::handleClickEvent(event); // Keep default behavior

        if (event.getType() == touchgfx::ClickEvent::PRESSED && onPressCallback) {
            onPressCallback->execute();
        } else if (event.getType() == touchgfx::ClickEvent::RELEASED && onReleaseCallback) {
            onReleaseCallback->execute();
        }
    }

private:
    GenericCallback<>* onPressCallback;
    GenericCallback<>* onReleaseCallback;
};
#endif /* GUI_INCLUDE_GUI_COMMON_CUSTOMBUTTON_HPP_ */
