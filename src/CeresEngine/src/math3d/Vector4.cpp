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

#include "Vector4.h"

#include <math.h>

using namespace ceres;

Vector4::Vector4() : Vector4( 0.0, 0.0, 0.0, 0.0 ) {}

Vector4::Vector4( scalar x, scalar y, scalar z, scalar w ) {

	set( x, y, z, w );

}

Vector4::Vector4( scalar xyzw ) {

	set( xyzw, xyzw, xyzw, xyzw );

}

Vector4::~Vector4() {}

Vector4 *Vector4::cero() {

	x = y = z = w = 0.0;

	return this;

}

bool Vector4::isCero() {

	return ( x == 0.0 ) && ( y == 0.0 ) && ( z == 0.0 ) && ( w == 0.0 );

}

Vector4 *Vector4::set( scalar x, scalar y, scalar z, scalar w ) {

	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;

	return this;

}

Vector4 *Vector4::copyToArray( scalar *dest, int32_t offset ) {

	dest[ offset ] = x;
	dest[ offset + 1 ] = y;
	dest[ offset + 2 ] = z;
	dest[ offset + 3 ] = w;

	return this;
}

Vector4 *Vector4::copyFromArray( scalar *source, int32_t offset ) {

	x = source[ offset ];
	y = source[ offset + 1 ];
	z = source[ offset + 2 ];
	w = source[ offset + 3 ];

	return this;

}

std::string Vector4::toString() {

	return std::string( "[ " ) + std::to_string( x ) + ", " + std::to_string( y ) + ", " + std::to_string( z ) + ", " + std::to_string( w ) + "]";

}
