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

#include "RadiusCollisionable.h"

using namespace ceres;

RadiusCollisionable::RadiusCollisionable():
	position( NULL ),
	radius( 0.0 ) {
}

RadiusCollisionable::~RadiusCollisionable() {
}

bool RadiusCollisionable::isPointInside( Vector3 *point ) {
	return position->distanceTo( point ) <= this->radius;
}

bool RadiusCollisionable::intersectsSphere( Vector3 *center, float radius ) {
	return position->distanceTo( center ) <= this->radius + radius;
}

bool RadiusCollisionable::castRay( Vector3 *origin, Vector3 *ray, float *intersectionParam, Vector3 *intersectionNormal ) {
	return false;
}
