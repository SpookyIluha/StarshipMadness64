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

#include "MathUtils.h"

namespace ceres {

class Vector3 {
public:

	Vector3();
	Vector3( Vector3 *a );
	Vector3( scalar x, scalar y, scalar z );
	Vector3( scalar xyz );
	~Vector3();

    Vector3 *cero();
	bool isCero();
	Vector3 *set( scalar x, scalar y, scalar z );
	Vector3 *copy( Vector3 *src );
	Vector3 *inc( Vector3 *a );
	Vector3 *inc( scalar x, scalar y, scalar z );
	Vector3 *dec( Vector3 *a );
	Vector3 *dec( scalar x, scalar y, scalar z );
	Vector3 *add( Vector3 *a, Vector3 *b );
	Vector3 *sub( Vector3 *a, Vector3 *b );
	Vector3 *negate();
	Vector3 *multiplyScalar( scalar s );
	Vector3 *multiply( Vector3 *a, Vector3 *b );
	Vector3 *divide( Vector3 *a, Vector3 *b );
	scalar dotProduct( Vector3 *b );
	Vector3 *crossProduct( Vector3 *a, Vector3 *b );
	scalar length();
	scalar distanceTo( Vector3 *a );
	Vector3 *normalize();
	scalar normalizeAndModulus();
	Vector3 *lerp( Vector3 *a, Vector3 *b, scalar f );
	Vector3 *clamp( Vector3 *min, Vector3 *max );

	Vector3 *copyToArray( scalar *dest, int32_t offset = 0 );
	Vector3 *copyFromArray( scalar *source, int32_t offset = 0 );

	static const Vector3 origin() { return Vector3( 0.0, 0.0, 0.0 ); }
	static const Vector3 xAxis() { return Vector3( 1.0, 0.0, 0.0 ); }
	static const Vector3 yAxis() { return Vector3( 0.0, 1.0, 0.0 ); }
	static const Vector3 zAxis() { return Vector3( 0.0, 0.0, 1.0 ); }
	static const Vector3 xyzAxis() { return Vector3( 1.0, 1.0, 1.0 ); }
	static const Vector3 minusXAxis() { return Vector3( - 1.0, 0.0, 0.0 ); }
	static const Vector3 minusYAxis() { return Vector3( 0.0, - 1.0, 0.0 ); }
	static const Vector3 minusZAxis() { return Vector3( 0.0, 0.0, - 1.0 ); }

	scalar maxDimension();

	Vector3 *reflect( Vector3 *incident, Vector3 *normal );

	std::string toString();

	scalar x, y, z;

private:

};

}
