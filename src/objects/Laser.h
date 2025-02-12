/*
 * Copyright © 2020 - 2024 yomboprime https://codeberg.org/yombo
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

#include "fixedPipeline/FPObject3D.h"

namespace ceres {

class Laser : public FPObject3D {
public:

	Laser();
	virtual ~Laser();

	virtual void setupRender( Camera *camera ) override;
	virtual void finishRender() override;
	virtual void render( Camera *camera ) override;

	void drawQuad( float *v, float *normal );

	Vector3 beginPoint, ray, rayvis;
	float thickness;

};

}
