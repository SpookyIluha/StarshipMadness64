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

#include "fixedPipeline/FPObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "math3d/Group.h"

namespace ceres {

class GL1ObjectUtils : public FPObjectUtils {
public:

	GL1ObjectUtils();
	virtual ~GL1ObjectUtils();

	GL1Mesh *createObject(
		std::string modelPath,
		GL1Material *material,
		float radius,
		Vector3 position,
		std::string &error,
		GL1Mesh *object = new GL1Mesh(),
		float scale = 1.0,
		bool smoothNormals = false
	);

	GL1Mesh *createPNCObject(std::string modelPath,
		GL1Material *material,
		float radius,
		Vector3 position,
		std::string &error,
		GL1Mesh *object = NULL,
		float scale = 1.0,
		bool smoothNormals = false
	);

};

}
