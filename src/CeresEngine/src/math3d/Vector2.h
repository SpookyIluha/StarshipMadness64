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

class Vector2 {
public:

	Vector2();
	Vector2( Vector2 *a );
	Vector2( scalar x, scalar y );
	Vector2( scalar xy );
	~Vector2();

    Vector2 *cero();
	bool isCero();
	Vector2 *set( scalar x, scalar y );
	Vector2 *copy( Vector2 *src );
	Vector2 *inc( Vector2 *a );
	Vector2 *inc( scalar x, scalar y );
	Vector2 *dec( Vector2 *a );
	Vector2 *dec( scalar x, scalar y );
	Vector2 *add( Vector2 *a, Vector2 *b );
	Vector2 *sub( Vector2 *a, Vector2 *b );
	Vector2 *negate();
	Vector2 *multiplyScalar( scalar s );
	scalar dotProduct( Vector2 *b );
	Vector2 *crossProduct( Vector2 *a, Vector2 *b );
	scalar length();
	scalar lengthSquared();
	scalar distanceTo( Vector2 *a );
	Vector2 *normalize();
	scalar normalizeAndModulus();
	Vector2 *lerp( Vector2 *a, Vector2 *b, scalar f );
	Vector2 *clamp( Vector2 *min, Vector2 *max );

	Vector2 *copyToArray( scalar *dest, int32_t offset = 0 );
	Vector2 *copyFromArray( scalar *source, int32_t offset = 0 );

	scalar maxDimension();

	std::string toString();

	scalar x, y;

private:

};

}
