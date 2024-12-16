/*
 * TouchXYCursor.hpp
 *
 *  Created on: Dec 16, 2024
 *      Author: apajadon
 */

#ifndef GUI_SRC_CUSTOM_TOUCHXYCURSOR_HPP_
#define GUI_SRC_CUSTOM_TOUCHXYCURSOR_HPP_

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Image.hpp>

class TouchXYCursor : public touchgfx::Widget
{
public:
	TouchXYCursor();
	/**
	 * @fn virtual touchgfx::Rect Lens::getSolidRect() const;
	 *
	 * @brief Report the solid dimensions of this widget.
	 *        As we would like the frame buffer below to be drawn,
	 *        the widget is reported as transparent
	 *
	 * @return an empty rectangle.
	 */
	virtual touchgfx::Rect getSolidRect() const;

	/**
	 * @fn virtual void Lens::draw(const touchgfx::Rect& invalidatedArea) const;
	 *
	 * @brief read, distort and write the frame buffer contents.
	 *
	 * @param invalidatedArea The part of the widget that should be drawn.
	 */
	virtual void draw(const touchgfx::Rect& invalidatedArea) const;

	/**
	 * @fn void Lens::setLensBackgroundBitmap(const touchgfx::BitmapId id);
	 *
	 * @brief set the bitmap that surrounds the lens part of the widget.
	 *
	 * @param id the bitmap to use.
	 */
	void setBackgroundBitmap(const touchgfx::BitmapId id);

	void setCollider(int x, int y, int width, int height);

	int getCenterX() const;
	int getCenterY() const;

private:
	void apply(uint16_t* fb, int absx, int absy, int x, int y) const;
	mutable int coord_x;       // X-coordinate of the top-left corner
	mutable int coord_y;       // Y-coordinate of the top-left corner
	mutable int coord_width;   // Width of the distortion
	mutable int coord_height;  // Height of the distortion

	int collider_xMin;
	int collider_xMax;
	int collider_yMin;
	int collider_yMax;

	static const uint8_t touchXYCursorRadius = 50;
	static const uint8_t touchXYCursorWidth = 2 * touchXYCursorRadius;
	static const uint8_t touchXYCursorZoom = 40;
	static const uint8_t touchXYCursorAlpha = 100;
	int16_t touchXYCursor[touchXYCursorWidth][touchXYCursorWidth]; //note: move the lens data to flash
	touchgfx::Image touchXYCursorImage;

};

#endif /* GUI_SRC_CUSTOM_TOUCHXYCURSOR_HPP_ */
