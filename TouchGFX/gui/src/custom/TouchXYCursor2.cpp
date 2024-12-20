/*
 * TouchXYCursor.cpp
 *
 *  Created on: Dec 16, 2024
 *      Author: apajadon
 */

#include "Custom/TouchXYCursor2.hpp"
#include <touchgfx/hal/HAL.hpp>
#include <math.h>
#include <BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

TouchXYCursor2::TouchXYCursor2()
: touchXYCursorImage(), collider_xMin(0), collider_xMax(0),
  collider_yMin(0), collider_yMax(0)
{
    setWidth(touchXYCursorWidth);
    setHeight(touchXYCursorWidth);
}

touchgfx::Rect TouchXYCursor2::getSolidRect() const
{
    return touchgfx::Rect();
}

void TouchXYCursor2::draw(const touchgfx::Rect& invalidatedArea) const
{
    touchgfx::Rect abs(0, 0, getWidth(), getHeight());
    translateRectToAbsolute(abs);

    coord_x = abs.x;
    coord_y = abs.y;
    coord_width = abs.width;
    coord_height = abs.height;

    if(coord_x < collider_xMin){
    	coord_x = collider_xMin;
    }
    if(coord_x + coord_width > collider_xMax){
    	coord_x = collider_xMax - coord_width;
    }
    if(coord_y < collider_yMin){
    	coord_y = collider_yMin;
    }
    if(coord_y + coord_height > collider_yMax){
    	coord_y = collider_yMax - coord_height;
    }

    // Limiter la zone de rendu au collider
    touchgfx::Rect constrainedArea(coord_x, coord_y, coord_width, coord_height);
//    touchgfx::Rect finalDirtyArea = constrainedArea & invalidatedArea;
    touchgfx::Rect finalDirtyArea = invalidatedArea;

//    if (!finalDirtyArea.isEmpty()) // S'assurer qu'il y a une zone valide à dessiner
//    {
        touchgfx::Rect dirtyBitmapArea = touchgfx::Bitmap(touchXYCursorImage.getBitmap()).getRect() & finalDirtyArea;
        touchgfx::HAL::lcd().drawPartialBitmap(
            touchgfx::Bitmap(touchXYCursorImage.getBitmap()),
            coord_x, coord_y,
            dirtyBitmapArea,
            touchXYCursorAlpha
        );
//    }


}

void TouchXYCursor2::setBackgroundBitmap(const touchgfx::BitmapId id)
{
	touchXYCursorImage.setBitmap(touchgfx::Bitmap(id));
}

void TouchXYCursor2::setCollider(int x, int y, int width, int height)
{
	collider_xMin = x;
	collider_xMax = x + width;
	collider_yMin = y;
	collider_yMax = y + height;
}


// Getter for the center X-coordinate
int TouchXYCursor2::getCenterX() const
{
    return coord_x + coord_width / 2;
}

// Getter for the center Y-coordinate
int TouchXYCursor2::getCenterY() const
{
    return coord_y + coord_height / 2;
}
