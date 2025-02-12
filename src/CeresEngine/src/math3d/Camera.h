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

#include "Posable.h"

namespace ceres {

class Camera : public Posable {
public:

	Camera();
	Camera( scalar minZ, scalar maxZ, scalar fovYDegrees, scalar aspect );
	~Camera();

	void updateProjectionMatrix();
	void updatePoseMatrixInverse();

	static const int CAMERA_ORTHO = 0;
	static const int CAMERA_CONIC = 1;

	int type;

	// Common parameters
	scalar minZ;
	scalar maxZ;

	// Only for conic projection
	scalar fovYDegrees;
	scalar aspect;

	// Only for orthogonal projection

	// Matrices
	Matrix4 poseMatrixInverse;
	Matrix4 projectionMatrix;

private:

};

}
