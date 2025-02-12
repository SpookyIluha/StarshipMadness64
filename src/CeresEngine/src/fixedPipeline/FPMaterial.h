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

#include "math3d/MathUtils.h"
#include "image/Image.h"
#include "FPTexture.h"

namespace ceres {

class FPMaterial {
public:

	FPMaterial() :
		illuminated( true ),
		emission( GraphicsColor( 0.0, 0.0, 0.0 ) ),
		ambient( GraphicsColor( 0.2, 0.2, 0.2 ) ),
		diffuse( GraphicsColor( 0.8, 0.8, 0.8) ),
		specular( GraphicsColor( 0.8, 0.8, 0.8 ) ),
		specularExponent( 0.0 ),
		doubleSided( false ),
		flatShading( true ),
		//wireframe( false ),
		transparent( false ),
		opacity( 1.0 ),
		depthTest( true ),
		texture( NULL ) {
	}

	virtual ~FPMaterial() {}

	virtual void beginUse() {}
	virtual void endUse() {}

	bool illuminated;

	GraphicsColor emission;
	GraphicsColor ambient;
	GraphicsColor diffuse;
	GraphicsColor specular;
	scalar specularExponent;

	bool doubleSided;
	bool flatShading;
	//bool wireframe;

	bool transparent;
	float opacity;

	bool depthTest;

	FPTexture *texture;

};

}
