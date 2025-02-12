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

#include "FPTexture.h"

using namespace ceres;

FPTexture::FPTexture() :
	magLinearFilter( false ),
	repeat( true ),
	bufferSizeBytes( 0 ),
	data( NULL ) {
}

FPTexture::~FPTexture() {
	sprite_free(data);
}

int32_t FPTexture::getPixelSizeBytes( int32_t numComponents ) {

	// float pixel type
	return numComponents * 4;

}


bool FPTexture::init( bool magLinearFilter, bool minLinearFilter, bool mipmap, sprite_t *data, std::string &error ) {
	return false;
}

float *FPTexture::createBuffer( int32_t resolutionX, int32_t resolutionY, int32_t numComponents ) {

	return new float[ numComponents * resolutionX * resolutionY ];

}

void FPTexture::upload( sprite_t *src ) {
}
