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

#include <vector>
#include <string>
#include <map>

#include "math3d/Vector2.h"
#include "math3d/Camera.h"
#include "fixedPipeline/FPObject3D.h"
#include "fixedPipeline/FPTexture.h"
#include "fixedPipeline/FPFont.h"

namespace ceres {

class GL1Text3D : public FPObject3D {
public:

	GL1Text3D();
	virtual ~GL1Text3D();

	void init( FPFont *font, FPMaterial *material, Vector3 scale );
	void setString( std::string s );

	virtual void setupRender( Camera *camera ) override;
	virtual void finishRender() override;
	virtual void render( Camera *camera ) override;


	// Scale ( z is not used)
	Vector3 scale;

	// Font
	FPFont *font;

	// Current content string
	std::string currentContentString;

	// Current string in indexes
	int32_t *charsIndexes;
	int32_t maxChars;
	int32_t currentChars;

	int32_t maxCharsX;
	int32_t maxCharsY;

};

}
