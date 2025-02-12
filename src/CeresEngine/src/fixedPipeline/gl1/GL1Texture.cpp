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
//#include <GL/glext.h>
#include <GL/gl_integration.h>

using namespace ceres;

GL1Texture::GL1Texture() :
	FPTexture(),
	target( GL_TEXTURE_2D ),
	textureName( 0 ) {
}

GL1Texture::~GL1Texture() {

	if ( textureName != 0 ) glDeleteTextures( 1, &textureName );
	textureName = 0;

}

void GL1Texture::create( int32_t target ) {

	this->target = target;
	GLuint texname = 0;
	glGenTextures( 1, &texname );
	textureName = texname;

	glBindTexture( target, textureName );

}

bool GL1Texture::init( bool magLinearFilter, bool minLinearFilter, bool mipmap, sprite_t *data, std::string &error ) {

	// data can be null (no data upload at this moment)

	GLenum textureTarget = GL_TEXTURE_2D;

	return initInternal( textureTarget, magLinearFilter, minLinearFilter, mipmap, data, error );

}

bool GL1Texture::initInternal(
	int32_t target,
	bool magLinearFilter,
	bool minLinearFilter,
	bool mipmap,
	sprite_t *data,
	std::string &error ) {



	if ( !data) {

		error = std::string( "GL1Texture::initInternal(): Error the sprite_t data is NULL." );
		return false;

	}

	target = GL_TEXTURE_2D;
	create( target );

	surface_t surf = sprite_get_pixels(data);

	this->resolutionX = surf.width;
	this->resolutionY = surf.height;
	this->numComponents = sprite_get_format(data);

	this->magLinearFilter = magLinearFilter;
	this->minLinearFilter = minLinearFilter;
	this->hasMipmap = sprite_get_lod_count(data) > 1;

	if ( data ) upload( data );
	this->data = data;

	memset(&this->texparms, 0, sizeof(rdpq_texparms_t));
	this->texparms.s.repeats = repeat? REPEAT_INFINITE : 1;
	this->texparms.t.repeats = repeat? REPEAT_INFINITE : 1;
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, magLinearFilter ? GL_LINEAR : GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, minLinearFilter ? GL_LINEAR : GL_NEAREST );

	//glTexParameteri( target, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP );
	//glTexParameteri( target, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP );

	return true;

}

void GL1Texture::upload(  sprite_t *src ) {
	surface_t surf = sprite_get_pixels(src);
	rdpq_texparms_t parms = {0};
	parms.s.repeats = REPEAT_INFINITE;
	parms.t.repeats = REPEAT_INFINITE;
	glSurfaceTexImageN64(GL_TEXTURE_2D, 0, &surf, &parms);
	/*GLint internalFormat = 0;
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

	}*/

}
