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

#include "Image.h"

#include "math3d/MathUtils.h"

#include <stdio.h>
#include <cstring>

using namespace ceres;


Image::Image() {

	this->width = 0;
	this->height = 0;
	this->numChannels = 0;
	this->data = NULL;

}

Image::Image( int32_t width, int32_t height, int32_t numChannels, float *data ) {


	this->width = width;
	this->height = height;
	this->numChannels = numChannels;
	this->data = data;

	if ( ! data ) allocateData();

}

Image::Image( Image *src ) {

	this->width = src->width;
	this->height = src->height;
	this->numChannels = src->numChannels;
	this->data = NULL;

	allocateData();
	copy( src );

}

Image::~Image() {

	if ( data ) delete [] data;

}

bool Image::allocateData() {

	if ( data ) delete [] data;
	data = new float[ width * height * numChannels ];
	if ( data ) fillInBlack();
	return data != NULL;

}

bool Image::loadFromPPMFile( std::string path ) {

	// Open file
	FILE *pFile;
	pFile = fopen( path.c_str(), "r" );
	if ( pFile == NULL ) return false;

	uint8_t theByte = 0;

	// Read P6
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != 'P' ) { fclose( pFile ); return false; }
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '6' ) { fclose( pFile ); return false; }
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '\n' ) { fclose( pFile ); return false; }

	// Skip comment line
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte == '#' ) {
		while ( theByte != '\n' ) if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	}

	// Read width
	std::string widthString = "";
	if ( theByte != ' ' ) widthString.push_back( theByte );
	while ( theByte != ' ' ) {

		if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
		if ( theByte != ' ' ) widthString.push_back( theByte );

	}

	// Read height
	std::string heightString = "";
	while ( theByte != '\n' ) {

		if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
		if ( theByte != '\n' ) heightString.push_back( theByte );

	}

	// Read 255
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '2' ) { fclose( pFile ); return false; }
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '5' ) { fclose( pFile ); return false; }
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '5' ) { fclose( pFile ); return false; }
	if ( fread( &theByte, 1, 1, pFile ) == 0 ) { fclose( pFile ); return false; }
	if ( theByte != '\n' ) { fclose( pFile ); return false; }

	int32_t w = std::stoul( widthString );
	int32_t h = std::stoul( heightString );

	if ( w <= 0 || h <= 0 ) { fclose( pFile ); return false; }

	if ( ! data || w != width || h != height ) {

		if ( numChannels == 0 ) numChannels = 3;
		width = w;
		height = h;
		allocateData();

	}

	size_t numPixels = getNumPixels();
	size_t numBytes = numPixels * 3;
	uint8_t *intData = new uint8_t[ numBytes ];
	if ( fread( intData, 1, numBytes, pFile ) < numBytes ) { delete [] intData; fclose( pFile ); return false; }

	int32_t p = 0;
	int32_t p2 = 0;
	for ( size_t i = 0; i < numPixels; i ++ ) {

		data[ p ] = ( (float) intData[ p2 ] ) / 255.0;
		data[ p + 1 ] = ( (float) intData[ p2 + 1 ] ) / 255.0;
		data[ p + 2 ] = ( (float) intData[ p2 + 2 ] ) / 255.0;
		if ( numChannels == 4 ) data[ p + 3 ] = 1.0;

		p += numChannels;
		p2 += 3;

	}
	delete [] intData;

	fclose( pFile );

	return true;
}

bool Image::saveToPPMFile( std::string path ) {

	if ( numChannels != 1 && numChannels < 3 ) return false;

	// Open file
	FILE *pFile;
	pFile = fopen( path.c_str(), "wb" );
	if ( pFile == NULL ) return false;

	// Write header
	fprintf( pFile, "P6\n%d %d\n255\n", (int)width, (int)height );

	// Write pixel data
	float *p = data;
	for( int y = 0; y < height; y++ ) {

		for( int x = 0; x < width; x++ ) {

			float r = maxim( 0.0, minim( 1.0, p[ 0 ] ) );
			float g = maxim( 0.0, minim( 1.0, numChannels == 1 ? r : p[ 1 ] ) );
			float b = maxim( 0.0, minim( 1.0, numChannels == 1 ? r : p[ 2 ] ) );

			uint8_t ri = ( uint8_t )( r * 255.0 );
			uint8_t gi = ( uint8_t )( g * 255.0 );
			uint8_t bi = ( uint8_t )( b * 255.0 );

			p += numChannels;

			fwrite( &ri, 1, 1, pFile );
			fwrite( &gi, 1, 1, pFile );
			fwrite( &bi, 1, 1, pFile );

		}

	}

	// Close file
	fclose( pFile );

	return true;

}

bool Image::copy( Image *src ) {

	if ( ! data || ! src->data ||
		numChannels != src->numChannels ||
		width != src->width ||
		height != src->height ) return false;

	memcpy( data, src->data, getNumBytes() );

	return true;

}

int32_t Image::getNumPixels() {

	return width * height;

}

int32_t Image::getNumBytesPerPixel() {

	return numChannels * 4;

}

int32_t Image::getNumBytes() {

	return getNumPixels() * numChannels * 4;

}

void Image::fillInBlack() {

	if ( data ) memset( data, 0, getNumBytes() );

}

void Image::fillInGray( float grayLevel ) {

	if ( ! data ) return;

	int32_t numPixels = getNumPixels();
	int32_t p = 0;
	for ( int32_t i = 0; i < numPixels; i ++ ) {

		for ( int32_t ch = 0; ch < numChannels; ch ++ ) data[ p  + ch ] = grayLevel;

		p += numChannels;

	}

}

void Image::fillInColor( GraphicsColor color ) {

	if ( ! data ) return;

	if ( numChannels == 3 ) {

		float rgb[ 3 ];
		color.toArray( rgb, false );
		int32_t numPixels = getNumPixels();
		float *pixel = data;
		for ( int32_t i = 0; i < numPixels; i ++ ) {
			setRGB( pixel, rgb );
			pixel += numChannels;
		}

	} else if ( numChannels == 4 ) {

		float rgba[ 4 ];
		color.toArray( rgba, true );
		int32_t numPixels = getNumPixels();
		float *pixel = data;
		for ( int32_t i = 0; i < numPixels; i ++ ) {
			setRGBA( pixel, rgba );
			pixel += numChannels;
		}

	}

}

void Image::normalize() {

	if ( ! data || numChannels != 1 ) return;

	float maxValue = fabs( data[ 0 ] );
	float minValue = maxValue;
	int32_t numPixels = getNumPixels();
	for ( int32_t i = 1; i < numPixels; i ++ ) {

		float v = fabs( data[ i ] );
		if ( v > maxValue ) maxValue = v;
		if ( v < minValue ) minValue = v;

	}

	if ( maxValue == 0.0 ) maxValue = 1.0;

	float range = maxValue - minValue;
	if ( range == 0.0 ) range = 1.0;

	for ( int32_t i = 0; i < numPixels; i ++ ) {

		data[ i ] = ( data[ i ] - minValue ) / range;

	}

}

void Image::multiplyScalar( float s ) {

	if ( ! data ) return;

	int32_t numPixels = getNumPixels();
	int32_t p = 0;
	for ( int32_t i = 0; i < numPixels; i ++ ) {

		for ( int32_t ch = 0; ch < numChannels; ch ++ ) data[ p  + ch ] *= s;

		p += numChannels;

	}

}

void Image::clamp() {

	if ( ! data || numChannels != 1 ) return;

	int32_t numPixels = getNumPixels();
	for ( int32_t i = 0; i < numPixels; i ++ ) {

		data[ i ] = minim( 1.0, maxim( 0.0, data[ i ] ) );

	}

}

bool Image::paintInImage( int32_t x, int32_t y, Image *screen ) {

	if ( ! data || ! screen->data ) return false;

	if ( numChannels != screen->numChannels ) return false;

	for ( int32_t j = 0; j < height; j ++ ) {

		for ( int32_t i = 0; i < width; i ++ ) {

			int32_t xs = x + i;
			int32_t ys = y + j;
			if ( xs >= 0 && xs < screen->width && ys >= 0 && ys < screen->height ) {

				int32_t p = getPixelOffset( i, j ) * numChannels;
				int32_t p2 = screen->getPixelOffset( xs, ys ) * numChannels;
				for ( int32_t ch = 0; ch < numChannels; ch ++ ) {

					screen->data[ p2 + ch ] = data[ p + ch ];

				}

			}

		}

	}

	return true;

}

bool Image::smooth( Image *dest ) {

	if ( ! data ) return false;

	if ( numChannels != 1 ) return false;

	if ( width != dest->width || height != dest->height || numChannels != dest->numChannels ) return false;

	int32_t p = 0;
	for ( int32_t j = 0; j < height; j ++ ) {

		for ( int32_t i = 0; i < width; i ++ ) {

			int32_t numNeighbours = 1;
			float sum = data[ p ];

			if ( i > 0 ) {

				sum += data[ p - 1 ];
				numNeighbours ++;

			}
			if ( i < width - 1 ) {

				sum += data[ p + 1 ];
				numNeighbours ++;

			}
			if ( j > 0 ) {

				sum += data[ p - width ];
				numNeighbours ++;

			}
			if ( j < height - 1 ) {

				sum += data[ p + width ];
				numNeighbours ++;

			}

			dest->data[ p ] = sum / numNeighbours;


			p ++;

		}

	}

	return true;

}

bool Image::smoothColor( Image *dest ) {

	if ( ! data ) return false;

	if ( numChannels != 3 ) return false;

	if ( width != dest->width || height != dest->height || numChannels != dest->numChannels ) return false;

	int32_t p = 0;
	for ( int32_t j = 0; j < height; j ++ ) {

		for ( int32_t i = 0; i < width; i ++ ) {

			int32_t numNeighbours = 1;
			float sumR = data[ p ];
			float sumG = data[ p + 1 ];
			float sumB = data[ p + 2 ];

			if ( i > 0 ) {

				sumR += data[ p - 3 ];
				sumG += data[ p - 3 + 1 ];
				sumB += data[ p - 3 + 2 ];
				numNeighbours ++;

			}
			if ( i < width - 1 ) {

				sumR += data[ p + 3 ];
				sumG += data[ p + 3 + 1 ];
				sumB += data[ p + 3 + 2 ];
				numNeighbours ++;

			}
			if ( j > 0 ) {

				sumR += data[ p - width * 3 ];
				sumG += data[ p - width * 3 + 1 ];
				sumB += data[ p - width * 3 + 2 ];
				numNeighbours ++;

			}
			if ( j < height - 1 ) {

				sumR += data[ p + width * 3 ];
				sumG += data[ p + width * 3 + 1 ];
				sumB += data[ p + width * 3 + 2 ];
				numNeighbours ++;

			}

			dest->data[ p ] = sumR / numNeighbours;
			dest->data[ p + 1 ] = sumG / numNeighbours;
			dest->data[ p + 2 ] = sumB / numNeighbours;


			p += 3;

		}

	}

	return true;

}

bool Image::convertRGBToGray( Image *dest ) {

	if ( ! data || numChannels < 3 ||
		! dest->data || dest->numChannels != 1 ||
		width != dest->width ||
		height != dest->height ) return false;

	int32_t numPixels = getNumPixels();
	int32_t p = 0;
	for ( int32_t i = 0; i < numPixels; i ++ ) {

		float gray = ( data[ p ] + data[ p + 1 ] + data[ p + 2 ] ) / 3.0;
		p += numChannels;
		dest->data[ i ] = gray;

	}

	return true;

}

bool Image::convertGrayToRGB( Image *dest ) {

	if ( ! dest->data || dest->numChannels < 3 ||
		! data || numChannels != 1 ||
		width != dest->width ||
		height != dest->height ) return false;

	int32_t numPixels = getNumPixels();
	int32_t p = 0;
	for ( int32_t i = 0; i < numPixels; i ++ ) {

		dest->data[ p ] = data[ i ];
		dest->data[ p + 1 ] = data[ i ];
		dest->data[ p + 2 ] = data[ i ];
		p += dest->numChannels;

	}

	return true;

}

uint32_t Image::getPixelOffset( uint32_t x, uint32_t y ) {

	return x + y * width;

}

float Image::getPixelChannelValue( uint32_t x, uint32_t y, uint32_t channel ) {

	return data[ x + y * width + channel ];

}

float Image::getGray( uint32_t x, uint32_t y ) {

	return data[ x + y * width ];

}

float Image::getR( uint32_t x, uint32_t y ) {

	return getPixelChannelValue(  x, y, 0 );

}

float Image::getG( uint32_t x, uint32_t y ) {

	return getPixelChannelValue(  x, y, 1 );

}

float Image::getB( uint32_t x, uint32_t y ) {

	return getPixelChannelValue(  x, y, 2 );

}

void Image::getRGB( uint32_t x, uint32_t y, float *rgb ) {

	float *p = data + numChannels * getPixelOffset( x, y );

	rgb[ 0 ] = *p ++;
	rgb[ 1 ] = *p ++;
	rgb[ 2 ] = *p ++;

}

void Image::getRGBA( uint32_t x, uint32_t y, float *rgba ) {

	float *p = data + numChannels * getPixelOffset( x, y );

	rgba[ 0 ] = *p ++;
	rgba[ 1 ] = *p ++;
	rgba[ 2 ] = *p ++;
	rgba[ 3 ] = *p ++;

}

void Image::setRGB( uint32_t x, uint32_t y, float *rgb ) {

	float *p = data + numChannels * getPixelOffset( x, y );

	*p ++ = rgb[ 0 ];
	*p ++ = rgb[ 1 ];
	*p ++ = rgb[ 2 ];

}

void Image::setRGBA( uint32_t x, uint32_t y, float *rgba ) {

	float *p = data + numChannels * getPixelOffset( x, y );

	*p ++ = rgba[ 0 ];
	*p ++ = rgba[ 1 ];
	*p ++ = rgba[ 2 ];
	*p ++ = rgba[ 3 ];

}

void Image::setRGB( float *p, float *rgb ) {

	*p ++ = rgb[ 0 ];
	*p ++ = rgb[ 1 ];
	*p ++ = rgb[ 2 ];

}

void Image::setRGBA( float *p, float *rgba ) {

	*p ++ = rgba[ 0 ];
	*p ++ = rgba[ 1 ];
	*p ++ = rgba[ 2 ];
	*p ++ = rgba[ 3 ];

}

GraphicsColor::GraphicsColor() {
	set( 0.0, 0.0, 0.0, 1.0 );
}

GraphicsColor::GraphicsColor( GraphicsColor *c ) {
	copy( c );
}

GraphicsColor GraphicsColor::black() {

	return GraphicsColor( 0.0, 0.0, 0.0 );

}

GraphicsColor::GraphicsColor( float r, float g, float b ) {
	set( r, g, b );
}

GraphicsColor::GraphicsColor( float r, float g, float b, float a ) {
	set( r, g, b, a );
}

void GraphicsColor::copy( GraphicsColor *c ) {
	set( c->r, c->g, c->b, c->a );
}

void GraphicsColor::set( float r, float g, float b ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0;
}

void GraphicsColor::set( float r, float g, float b, float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void GraphicsColor::toArray( float *dest, bool alpha ) {

	*dest ++ = r;
	*dest ++ = g;
	*dest ++ = b;
	if ( alpha ) *dest = a;

}
