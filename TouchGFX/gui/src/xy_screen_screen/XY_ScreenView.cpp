#include <gui/xy_screen_screen/XY_ScreenView.hpp>
#include <BitmapDatabase.hpp>

using touchgfx::Unicode;

XY_ScreenView::XY_ScreenView()
{

}

void XY_ScreenView::setupScreen()
{
    XY_ScreenViewBase::setupScreen();

    int x = TouchPicture.getX() + TouchPicture.getWidth()/2 - myTouchXYCursor.getWidth() / 2;
    int y = TouchPicture.getY() + TouchPicture.getHeight()/2 - myTouchXYCursor.getHeight() / 2;
    myTouchXYCursor.setXY(x, y);
    myTouchXYCursor.setCollider(TouchPicture.getX(), TouchPicture.getY(), TouchPicture.getWidth(), TouchPicture.getHeight());
    myTouchXYCursor.setBackgroundBitmap(BITMAP_CURSORXY2_ID);
    add(myTouchXYCursor);
}

void XY_ScreenView::tearDownScreen()
{
    XY_ScreenViewBase::tearDownScreen();
}


void XY_ScreenView::handleTickEvent()
{
	static int counter = 0;
	static int x_old = 0;
	static int y_old = 0;
	counter++;
	int x = myTouchXYCursor.getX();
	int y = myTouchXYCursor.getY();

	if((x_old == x) && (y_old == y)){
		return;
	}

	x_old = x;
	y_old = y;

	// Display XY coord
	if(counter % 2 == 0){
		Unicode::snprintf(coordX_lblBuffer, COORDX_LBL_SIZE, "%d", x);
		coordX_lbl.invalidate();

		Unicode::snprintf(coordY_lblBuffer, COORDY_LBL_SIZE, "%d", y);
		coordY_lbl.invalidate();

	}

	// Notify Presenter
	if(counter % 2 == 0){
		presenter->handleXYEvent(x, y);
	}
}



