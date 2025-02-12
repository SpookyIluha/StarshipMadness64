/*
 * Copyright © 2020 - 2025 yomboprime https://codeberg.org/yombo
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#pragma once

#include <inttypes.h>
#include <string>

namespace ceres {

// Generic storage for a pixel in floating point numbers
struct GraphicsColor {

	float r, g, b, a;
	/*
	union {
		struct {
			float r, g, b, a;
		} v;
		float rgba[ 4 ];
	};
	*/

	GraphicsColor();
	GraphicsColor( float r, float g, float b );
	GraphicsColor( float r, float g, float b, float a );
	GraphicsColor( GraphicsColor *c );

	static GraphicsColor black();

	void copy( GraphicsColor *c );
	void set( float r, float g, float b );
	void set( float r, float g, float b, float a );

	void toArray( float *dest, bool alpha );

};

class Image {

public:

	int32_t width;
	int32_t height;
	int32_t numChannels;
	float *data;

	Image();
	Image( int32_t width, int32_t height, int32_t numChannels, float *data = NULL );
	Image( Image *src );
	~Image();

	bool allocateData();

	bool loadFromPPMFile( std::string path );
	bool saveToPPMFile( std::string path );

	bool copy( Image *src );

	int32_t getNumPixels();
	int32_t getNumBytesPerPixel();
	int32_t getNumBytes();

	void fillInBlack();
	void fillInGray( float grayLevel );
	void fillInColor( GraphicsColor color );
	void normalize();
	void multiplyScalar( float s );
	void clamp();

	bool paintInImage( int32_t x, int32_t y, Image *screen );
	bool smooth( Image *dest );
	bool smoothColor( Image *dest );

	bool convertRGBToGray( Image *dest );
	bool convertGrayToRGB( Image *dest );

	uint32_t getPixelOffset( uint32_t x, uint32_t y );
	float getPixelChannelValue( uint32_t x, uint32_t y, uint32_t channel );

	float getGray( uint32_t x, uint32_t y );
	float getR( uint32_t x, uint32_t y );
	float getG( uint32_t x, uint32_t y );
	float getB( uint32_t x, uint32_t y );
	void getRGB( uint32_t x, uint32_t y, float *rgb );
	void getRGBA( uint32_t x, uint32_t y, float *argb );

	void setRGB( uint32_t x, uint32_t y, float *rgb );
	void setRGBA( uint32_t x, uint32_t y, float *argb );

	void setRGB( float *p, float *rgb );
	void setRGBA( float *p, float *argb );

};

}
