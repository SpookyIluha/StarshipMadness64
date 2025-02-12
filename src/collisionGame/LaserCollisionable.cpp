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

#include "LaserCollisionable.h"

using namespace ceres;

LaserCollisionable::LaserCollisionable() {
}

LaserCollisionable::~LaserCollisionable() {
}

bool LaserCollisionable::isPointInside( Vector3 *point ) {
	return false;
}

bool LaserCollisionable::intersectsSphere( Vector3 *center, float radius ) {

	if ( ! laser->visible ) return false;

	Vector3 relPos;
	relPos.sub( center, &laser->beginPoint );
	float d = relPos.length();
	if ( d <= radius ) return true;
	Vector3 ray1 = laser->ray;
	float r = ray1.normalizeAndModulus();
	float p = relPos.dotProduct( &ray1 );
	if ( p < 0 || p > r ) return false;
	ray1.multiplyScalar( p )->dec( &relPos );
	return ray1.length() <= radius;

}

bool LaserCollisionable::castRay( Vector3 *origin, Vector3 *ray, float *intersectionParam, Vector3 *intersectionNormal ) {
	return false;
}
