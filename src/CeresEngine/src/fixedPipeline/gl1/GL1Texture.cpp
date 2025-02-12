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

#include "GL1Texture.h"

#include <GL/gl.h>
#include <GL/glext.h>

using namespace ceres;

GL1Texture::GL1Texture() :
	FPTexture(),
	target( 0 ),
	textureName( 0 ) {
}

GL1Texture::~GL1Texture() {

	if ( textureName != 0 ) glDeleteTextures( 1, &textureName );
	textureName = 0;

}

void GL1Texture::create( int32_t target ) {

	this->target = target;

	glGenTextures( 1, &textureName );
	glBindTexture( target, textureName );

}

bool GL1Texture::init( int32_t resolutionX, int32_t resolutionY, int32_t numComponents, bool magLinearFilter, bool minLinearFilter, bool mipmap, void *data, std::string &error ) {

	// data can be null (no data upload at this moment)

	GLenum textureTarget = GL_TEXTURE_2D;

	return initInternal( textureTarget, resolutionX, resolutionY, numComponents, magLinearFilter, minLinearFilter, mipmap, data, error );

}

bool GL1Texture::initInternal(
	int32_t target,
	int32_t resolutionX,
	int32_t resolutionY,
	int32_t numComponents,
	bool magLinearFilter,
	bool minLinearFilter,
	bool mipmap,
	void *data,
	std::string &error ) {

	if ( resolutionX == 0 || resolutionY == 0 ) {

		error = std::string( "GL1Texture::initInternal(): Error defining texture: Texture resolution is 0 in some dimension/s." );
		return false;

	}

	if ( numComponents == 0 ) {

		error = std::string( "GL1Texture::initInternal(): Error defining texture: numComponents is 0." );
		return false;

	}

	if ( numComponents != 1 && numComponents != 3 && numComponents != 4 ) {

		error = std::string( "GL1Texture::initInternal(): Error defining texture: numComponents must be 1, 3 or 4." );
		return false;

	}

	create( target );

	this->resolutionX = resolutionX;
	this->resolutionY = resolutionY;
	this->numComponents = numComponents;

	this->magLinearFilter = magLinearFilter;
	this->minLinearFilter = minLinearFilter;
	this->hasMipmap = mipmap;

	bufferSizeBytes = getPixelSizeBytes( numComponents ) * resolutionX * resolutionY;

	if ( data ) upload( data );
	this->data = (float *)data;

	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, magLinearFilter ? GL_LINEAR : GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, minLinearFilter ? GL_LINEAR : GL_NEAREST );

	glTexParameteri( target, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE );

	return true;

}

void GL1Texture::upload( void *src ) {

	GLint internalFormat = 0;
	GLenum inputFormat = 0;
	switch ( numComponents ) {
		case 1:
			internalFormat = GL_RED;
			inputFormat = GL_RED;
			break;
		case 3:
			internalFormat = GL_RGB;
			inputFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA;
			inputFormat = GL_RGBA;
			break;
		default:
			return;
	}

	glBindTexture( target, textureName );

	if ( target == GL_TEXTURE_2D ) {

		glTexImage2D(
			target,
			0,
			internalFormat,
			resolutionX, resolutionY,
			0,
			inputFormat,
			//GL_FLOAT,
			GL_UNSIGNED_BYTE,
			src );

        // Currently REQUIRED for OpenGLdc w/PNGs on Dreamcast
		if ( this->hasMipmap ) glGenerateMipmapEXT( GL_TEXTURE_2D );

	}

}
