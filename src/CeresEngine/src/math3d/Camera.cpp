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

#include "Camera.h"

#include <stdio.h>	// debug, remove

using namespace ceres;

Camera::Camera(): Camera( 0.01, 500, 65.0, 640.0 / 480.0 ) {}

Camera::Camera( scalar minZ, scalar maxZ, scalar fovYDegrees, scalar aspect ) {

	type = CAMERA_CONIC;

	this->minZ = minZ;
	this->maxZ = maxZ;
	this->fovYDegrees = fovYDegrees;
	this->aspect = aspect;

	pose = new Pose();

	updateProjectionMatrix();

}

Camera::~Camera() {}

void Camera::updateProjectionMatrix() {

	switch ( type ) {

		case CAMERA_ORTHO:
			//TODO
			projectionMatrix.identity();
			break;

		case CAMERA_CONIC:

			projectionMatrix.conicProjection( fovYDegrees, aspect, minZ, maxZ );

			break;

		default:
			break;
	}

}

void Camera::updatePoseMatrixInverse() {

	poseMatrixInverse.inverseHomogeneous( &pose->matrix );

}
