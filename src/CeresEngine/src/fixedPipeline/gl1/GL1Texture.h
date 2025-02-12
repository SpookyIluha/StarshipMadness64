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

#include <string>

#include "fixedPipeline/FPTexture.h"

#include <GL/gl.h>

namespace ceres {

class GL1Texture : public FPTexture {
public:

	GL1Texture();
	virtual ~GL1Texture();

	virtual bool init(bool magLinearFilter, bool minLinearFilter, bool mipmap, sprite_t *data, std::string &error ) override;

	virtual void upload( sprite_t *src ) override;

	GLenum target;
	GLuint textureName;

protected:

	void create( int32_t target );

	bool initInternal( int32_t target, bool magLinearFilter, bool minLinearFilter, bool mipmap, sprite_t *data, std::string &error );

private:

};

}
