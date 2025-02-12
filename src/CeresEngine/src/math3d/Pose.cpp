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

#include "Pose.h"
#include <libdragon.h>

using namespace ceres;

Pose::Pose() {
	position = Vector3();
	rotation = Quaternion();
	matrix = Matrix4();
	scale = false;
}

Pose::~Pose() {}

Vector3 two = Vector3(2,2,2);
Vector3 one = Vector3(1,1,1);

void Pose::updateMatrix() {

	matrix.setRotationFromQuaternion( &rotation )->setTraslation( &position );
	matrix.scale(scale? &two : &one);

}
