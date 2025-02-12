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

struct Link {
	Link() {}
	Link( int32_t point0, int32_t point1 );
	virtual ~Link() {}

	int32_t point0, point1;
};

class Tesseract : public FPObject3D {
public:

	Tesseract();
	virtual ~Tesseract();

	virtual void setupRender( Camera *camera ) override;
	virtual void finishRender() override;
	virtual void render( Camera *camera ) override;

	void transformPoints();
	void drawQuad( float *v, float *normal );

	float size;
	std::vector<Vector3> points;
	std::vector<Vector3> rotatedPoints;
	std::vector<Vector3> transformedPoints;
	std::vector<Link> links;
	float thickness;

	Pose pose0;
	Pose pose1;

	rspq_block_t* block = NULL;

};

}
