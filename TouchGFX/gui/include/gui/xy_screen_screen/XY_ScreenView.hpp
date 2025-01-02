#ifndef XY_SCREENVIEW_HPP
#define XY_SCREENVIEW_HPP

#include <custom/TouchXYCursor2.hpp>
#include <custom/TouchXYCursor.hpp>
#include <gui_generated/xy_screen_screen/XY_ScreenViewBase.hpp>
#include <gui/xy_screen_screen/XY_ScreenPresenter.hpp>
#include <touchgfx/mixins/Draggable.hpp>

class XY_ScreenView : public XY_ScreenViewBase
{
public:
    XY_ScreenView();
    virtual ~XY_ScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    int getTouchPictureX() const 		{ return TouchPicture.getX();}
    int getTouchPictureY() const	    { return TouchPicture.getY();}
    int getTouchPictureWidth() const 	{ return TouchPicture.getWidth();}
    int getTouchPictureHeight() const   { return TouchPicture.getHeight();}
protected:
    Draggable<TouchXYCursor2> myTouchXYCursor;
};

#endif // XY_SCREENVIEW_HPP
