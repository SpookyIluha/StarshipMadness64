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

#include "BoxCollisionable.h"

using namespace ceres;

BoxCollisionable::BoxCollisionable() {
}

BoxCollisionable::~BoxCollisionable() {
}

bool BoxCollisionable::isPointInside( Vector3 *point ) {

	for ( int64_t i = 0, n = boxes.size(); i < n; i ++ ) {
		AABBox *box = boxes[ i ];
		if ( box->isPointInside( point ) ) {
			return true;
		}
	}
	return false;
}

bool BoxCollisionable::intersectsSphere( Vector3 *center, float radius ) {
	return false;
}

bool BoxCollisionable::castRay( Vector3 *origin, Vector3 *ray, float *intersectionParam, Vector3 *intersectionNormal ) {

	float step = 10.0;

	float rayLength = ray->length();
	Vector3 increm = ray;
	increm.multiplyScalar( 1.0 / step );
	Vector3 endPoint = origin;
	for ( int32_t i = 0, n = floor( rayLength / step ); i < n; i ++ ) {

		endPoint.inc( &increm );

		for ( int64_t i = 0, n = boxes.size(); i < n; i ++ ) {
			AABBox *box = boxes[ i ];
			if ( box->isPointInside( &endPoint ) ) {
				*intersectionParam = i * increm.length() / rayLength;
				intersectionNormal->cero();
				return true;
			}
		}

	}

	return false;
}
