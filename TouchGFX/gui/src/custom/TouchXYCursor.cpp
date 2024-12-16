/*
 * TouchXYCursor.cpp
 *
 *  Created on: Dec 16, 2024
 *      Author: apajadon
 */

#include "Custom/TouchXYCursor.hpp"
#include <touchgfx/hal/HAL.hpp>
#include <math.h>
#include <BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

TouchXYCursor::TouchXYCursor()
{
    // calculate the distortion of the TouchXYCursor
    // derivations of the formula can be found online, for instance in the demo effects collection at
    // http://demo-effects.cvs.sourceforge.net/viewvc/demo-effects/demo-effects/TouchXYCursor/TouchXYCursor.c?view=markup
    for (int y = 0; y < touchXYCursorRadius; y++)
    {
        for (int x = 0; x < touchXYCursorRadius; x++)
        {
            int ix, iy, offset;
            if ((x * x + y * y) < (touchXYCursorRadius * touchXYCursorRadius))
            {
                float shift = touchXYCursorZoom / (float)sqrt((float)touchXYCursorZoom * touchXYCursorZoom - (x * x + y * y - touchXYCursorRadius * touchXYCursorRadius));
                ix = (int)(x * shift - x);
                iy = (int)(y * shift - y);
            }
            else
            {
                ix = 0;
                iy = 0;
            }
            offset = (iy * touchgfx::HAL::FRAME_BUFFER_WIDTH + ix);
            touchXYCursor[touchXYCursorRadius - y][touchXYCursorRadius - x] = -offset;
            touchXYCursor[touchXYCursorRadius + y][touchXYCursorRadius + x] = offset;
            offset = (-iy * touchgfx::HAL::FRAME_BUFFER_WIDTH + ix);
            touchXYCursor[touchXYCursorRadius + y][touchXYCursorRadius - x] = -offset;
            touchXYCursor[touchXYCursorRadius - y][touchXYCursorRadius + x] = offset;
        }
    }
    setWidth(touchXYCursorWidth);
    setHeight(touchXYCursorWidth);
}

touchgfx::Rect TouchXYCursor::getSolidRect() const
{
    return touchgfx::Rect();
}

void TouchXYCursor::draw(const touchgfx::Rect& invalidatedArea) const
{

    touchgfx::Rect abs(0, 0, getWidth(), getHeight());
    translateRectToAbsolute(abs);

    coord_x = abs.x;
    coord_y = abs.y;
    coord_width = abs.width;
    coord_height = abs.height;

    if(coord_x < collider_xMin){
    	return;
    }
    if(coord_x + coord_width > collider_xMax){
    	return;
    }
    if(coord_y < collider_yMin){
    	return;
    }
    if(coord_y + coord_height > collider_yMax){
    	return;
    }

    // since we both read and write the frame buffer, we split the loops in to four sections/quadrants
    // and run through them left to right or right to left, top to bottom or bottom to top,
    int16_t middleX = getWidth() / 2;
    int16_t middleY = getHeight() / 2;

    uint16_t* fb = touchgfx::HAL::getInstance()->lockFrameBuffer();
    for (int y = invalidatedArea.y; y < invalidatedArea.bottom() && y <= middleY; y++)
    {
        for (int x = invalidatedArea.x; x < invalidatedArea.right() && x <= middleX; x++)
        {
            apply(fb, abs.x, abs.y, x, y);
        }

        for (int x = invalidatedArea.right() - 1; x > middleX && x >= invalidatedArea.x; x--)
        {
            apply(fb, abs.x, abs.y, x, y);
        }
    }

    for (int y = invalidatedArea.bottom() - 1; y > middleY && y >= invalidatedArea.y; y--)
    {
        for (int x = invalidatedArea.x; x < invalidatedArea.right() && x <= middleX; x++)
        {
            apply(fb, abs.x, abs.y, x, y);
        }

        for (int x = invalidatedArea.right() - 1; x > middleX && x >= invalidatedArea.x; x--)
        {
            apply(fb, abs.x, abs.y, x, y);
        }
    }

    touchgfx::HAL::getInstance()->unlockFrameBuffer();

    touchgfx::Rect dirtyBitmapArea = touchgfx::Bitmap(touchXYCursorImage.getBitmap()).getRect() & invalidatedArea;
    touchgfx::HAL::lcd().drawPartialBitmap(touchgfx::Bitmap(touchXYCursorImage.getBitmap()), abs.x, abs.y, dirtyBitmapArea, touchXYCursorAlpha);
}

void TouchXYCursor::setBackgroundBitmap(const touchgfx::BitmapId id)
{
	touchXYCursorImage.setBitmap(touchgfx::Bitmap(id));
}

void TouchXYCursor::setCollider(int x, int y, int width, int height)
{
	collider_xMin = x;
	collider_xMax = x + width;
	collider_yMin = y;
	collider_yMax = y + height;
}

void TouchXYCursor::apply(uint16_t* fb, int absx, int absy, int x, int y) const
{
    // switch on the pixel depth of the application
    // For 16 bit, the framebuffer pointer is already the correct size
    // For 24 bit, the frame buffer needs to be cast to a 8 bit size in order to access and manipulate each color independently
    if (touchgfx::HAL::lcd().bitDepth() == 16)
    {
        uint8_t* frameptr16 = (uint8_t*)fb;
        int16_t touchXYCursorOffset = touchXYCursor[y][x];
        uint32_t fbOffset = x + absx + (y + absy) * touchgfx::HAL::FRAME_BUFFER_WIDTH;
        if ((int32_t)fbOffset + touchXYCursorOffset < 0)
        {
            frameptr16[fbOffset * 2] = frameptr16[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 2];
            frameptr16[fbOffset * 2 + 1] = frameptr16[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 2 + 1];
        }
        else if ((int32_t)fbOffset + touchXYCursorOffset > touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH)
        {
            frameptr16[fbOffset * 2] = frameptr16[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 2];
            frameptr16[fbOffset * 2 + 1] = frameptr16[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 2 + 1];
        }
        else
        {
            frameptr16[fbOffset * 2] = frameptr16[(fbOffset + touchXYCursorOffset) * 2];
            frameptr16[fbOffset * 2 + 1] = frameptr16[(fbOffset + touchXYCursorOffset) * 2 + 1];
        }
    }
    else if (touchgfx::HAL::lcd().bitDepth() == 8)
    {
        uint8_t* frameptr24 = (uint8_t*)fb;
        int16_t touchXYCursorOffset = touchXYCursor[y][x];
        uint32_t fbOffset = x + absx + (y + absy) * touchgfx::HAL::FRAME_BUFFER_WIDTH;
        if ((int32_t)fbOffset + touchXYCursorOffset < 0)
        {
            frameptr24[fbOffset] = frameptr24[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH)];
        }
        else if ((int32_t)fbOffset + touchXYCursorOffset > touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH)
        {
            frameptr24[fbOffset] = frameptr24[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH)];
        }
        else
        {
            frameptr24[fbOffset] = frameptr24[(fbOffset + touchXYCursorOffset)];
        }
    }
    else /* assumed 24bit */
    {
        uint8_t* frameptr24 = (uint8_t*)fb;
        int16_t touchXYCursorOffset = touchXYCursor[y][x];
        uint32_t fbOffset = x + absx + (y + absy) * touchgfx::HAL::FRAME_BUFFER_WIDTH;
        if ((int32_t)fbOffset + touchXYCursorOffset < 0)
        {
            frameptr24[fbOffset * 3] = frameptr24[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3];
            frameptr24[fbOffset * 3 + 1] = frameptr24[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3 + 1];
            frameptr24[fbOffset * 3 + 2] = frameptr24[(fbOffset + touchXYCursorOffset + touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3 + 2];
        }
        else if ((int32_t)fbOffset + touchXYCursorOffset > touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH)
        {
            frameptr24[fbOffset * 3] = frameptr24[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3];
            frameptr24[fbOffset * 3 + 1] = frameptr24[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3 + 1];
            frameptr24[fbOffset * 3 + 2] = frameptr24[(fbOffset + touchXYCursorOffset - touchgfx::HAL::DISPLAY_HEIGHT * touchgfx::HAL::FRAME_BUFFER_WIDTH) * 3 + 2];
        }
        else
        {
            frameptr24[fbOffset * 3] = frameptr24[(fbOffset + touchXYCursorOffset) * 3];
            frameptr24[fbOffset * 3 + 1] = frameptr24[(fbOffset + touchXYCursorOffset) * 3 + 1];
            frameptr24[fbOffset * 3 + 2] = frameptr24[(fbOffset + touchXYCursorOffset) * 3 + 2];
        }
    }
}


// Getter for the center X-coordinate
int TouchXYCursor::getCenterX() const
{
    return coord_x + coord_width / 2;
}

// Getter for the center Y-coordinate
int TouchXYCursor::getCenterY() const
{
    return coord_y + coord_height / 2;
}
