#include <gui/xy_screen_screen/XY_ScreenView.hpp>
#include <BitmapDatabase.hpp>

using touchgfx::Unicode;

XY_ScreenView::XY_ScreenView()
{

}

void XY_ScreenView::setupScreen()
{
    XY_ScreenViewBase::setupScreen();
    myTouchXYCursor.setXY(200, 200);
    myTouchXYCursor.setBackgroundBitmap(BITMAP_CURSORXY2_ID);
    add(myTouchXYCursor);
}

void XY_ScreenView::tearDownScreen()
{
    XY_ScreenViewBase::tearDownScreen();
}


void XY_ScreenView::handleTickEvent()
{
    int x = myTouchXYCursor.getX();
    Unicode::snprintf(coordX_lblBuffer, COORDX_LBL_SIZE, "%d", x);
    coordX_lbl.invalidate();

    int y = myTouchXYCursor.getY();
    Unicode::snprintf(coordY_lblBuffer, COORDY_LBL_SIZE, "%d", y);
    coordY_lbl.invalidate();
}


