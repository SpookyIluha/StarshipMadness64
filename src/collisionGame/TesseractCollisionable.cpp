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

#include "TesseractCollisionable.h"

using namespace ceres;

TesseractCollisionable::TesseractCollisionable() {
}

TesseractCollisionable::~TesseractCollisionable() {
}

bool TesseractCollisionable::isPointInside( Vector3 *point ) {

	for ( int32_t i = 0, n = tesseract->links.size(); i < n; i ++ ) {

		Link *link = &tesseract->links[ i ];

		Vector3 *p0 = &tesseract->transformedPoints[ link->point0 ];
		Vector3 *p1 = &tesseract->transformedPoints[ link->point1 ];

		Vector3 p1p0;
		float l = p1p0.sub( p1, p0 )->normalizeAndModulus();
		Vector3 pos;
		pos.sub( point, p0 );
		float l0 = p1p0.dotProduct( &pos );
		if ( l0 < 0 || l0 > l ) return false;
		float d = sqrt( pow( pos.length(), 2.0 ) - pow( l0, 2.0 ) );
		if ( d <= thickness ) return true;

	}

	return false;

}

bool TesseractCollisionable::intersectsSphere( Vector3 *center, float radius ) {
	return false;
}

bool TesseractCollisionable::castRay( Vector3 *origin, Vector3 *ray, float *intersectionParam, Vector3 *intersectionNormal ) {

	float step = 10.0;

	float rayLength = ray->length();
	Vector3 increm = ray;
	increm.multiplyScalar( 1.0 / step );
	Vector3 endPoint = origin;
	for ( int32_t i = 0, n = floor( rayLength / step ); i < n; i ++ ) {

		endPoint.inc( &increm );

		if ( isPointInside( &endPoint ) ) {
			*intersectionParam = i * increm.length() / rayLength;
			intersectionNormal->cero();
			return true;
		}
	}

	return false;
}
