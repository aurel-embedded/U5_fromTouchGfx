/*
 * TouchXYCursor2.hpp
 *
 *  Created on: Dec 16, 2024
 *      Author: apajadon
 */

#ifndef GUI_SRC_CUSTOM_TOUCHXYCURSOR2_HPP_
#define GUI_SRC_CUSTOM_TOUCHXYCURSOR2_HPP_

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Image.hpp>

class TouchXYCursor2 : public touchgfx::Widget
{
public:
	TouchXYCursor2();
	virtual touchgfx::Rect getSolidRect() const;
	virtual void draw(const touchgfx::Rect& invalidatedArea) const;
	void setBackgroundBitmap(const touchgfx::BitmapId id);

	void setCollider(int x, int y, int width, int height);

	int getCenterX() const;
	int getCenterY() const;

private:
	mutable int coord_x;       // X-coordinate of the top-left corner
	mutable int coord_y;       // Y-coordinate of the top-left corner
	mutable int coord_width;   // Width of the distortion
	mutable int coord_height;  // Height of the distortion

	touchgfx::Image touchXYCursorImage;
	int collider_xMin;
	int collider_xMax;
	int collider_yMin;
	int collider_yMax;

	static const uint8_t touchXYCursorRadius = 50;
	static const uint8_t touchXYCursorWidth = 2 * touchXYCursorRadius;
	static const uint8_t touchXYCursorZoom = 40;
	static const uint8_t touchXYCursorAlpha = 100;
	int16_t touchXYCursor[touchXYCursorWidth][touchXYCursorWidth]; //note: move the lens data to flash

};

#endif /* GUI_SRC_CUSTOM_TOUCHXYCURSOR2_HPP_ */
