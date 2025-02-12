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

#include "Vector3.h"

#include <math.h>

using namespace ceres;

Vector3::Vector3() : Vector3( 0.0, 0.0, 0.0 ) {}

Vector3::Vector3( Vector3 *a ) {

	set( a->x, a->y, a->z );

}

Vector3::Vector3( scalar x, scalar y, scalar z ) {

	set( x, y, z );

}

Vector3::Vector3( scalar xyz ) {

	set( xyz, xyz, xyz );

}

Vector3::~Vector3() {}

Vector3 *Vector3::copy( Vector3 *src ) {

	return set( src->x, src->y, src->z );

}

Vector3 *Vector3::set( scalar x, scalar y, scalar z ) {

	this->x = x;
	this->y = y;
	this->z = z;

	return this;

}

Vector3 *Vector3::cero() {

	set( 0.0, 0.0, 0.0 );

	return this;
}

bool Vector3::isCero() {

	return ( x == 0.0 ) && ( y == 0.0 ) && ( z == 0.0 );

}

Vector3 *Vector3::inc( Vector3 *a ) {

	x += a->x;
	y += a->y;
	z += a->z;

	return this;

}

Vector3 *Vector3::inc( scalar x, scalar y, scalar z ) {

	this->x += x;
	this->y += y;
	this->z += z;

	return this;

}

Vector3 *Vector3::dec( Vector3 *a ) {

	x -= a->x;
	y -= a->y;
	z -= a->z;

	return this;

}

Vector3 *Vector3::dec( scalar x, scalar y, scalar z ) {

	this->x -= x;
	this->y -= y;
	this->z -= z;

	return this;

}

Vector3 *Vector3::add( Vector3 *a, Vector3 *b ) {

	x = a->x + b->x;
	y = a->y + b->y;
	z = a->z + b->z;

	return this;
}

Vector3 *Vector3::sub( Vector3 *a, Vector3 *b ) {

	x = a->x - b->x;
	y = a->y - b->y;
	z = a->z - b->z;

	return this;

}

Vector3 *Vector3::negate() {

	x = - x;
	y = - y;
	z = - z;

	return this;

}

Vector3 *Vector3::multiplyScalar( scalar s ) {

	x *= s;
	y *= s;
	z *= s;

	return this;

}

Vector3 *Vector3::multiply( Vector3 *a, Vector3 *b ) {

	x = a->x * b->x;
	y = a->y * b->y;
	z = a->z * b->z;

	return this;

}

Vector3 *Vector3::divide( Vector3 *a, Vector3 *b ) {

	x = a->x / b->x;
	y = a->y / b->y;
	z = a->z / b->z;

	return this;

}

scalar Vector3::dotProduct( Vector3 *b ) {

	return x * b->x + y * b->y + z * b->z;

}

Vector3 *Vector3::crossProduct( Vector3 *a, Vector3 *b ) {

	x = a->y * b->z - a->z * b->y;
	y = a->z * b->x - a->x * b->z;
	z = a->x * b->y - a->y * b->x;

	return this;

}

scalar Vector3::length() {

	return sqrt( x * x + y * y + z * z );

}

scalar Vector3::distanceTo( Vector3 *a ) {

	Vector3 d;
	d.sub( this, a );
	return d.length();

}

Vector3 *Vector3::normalize() {

	scalar d = length();

	if ( fabs( d ) < 0.00000001 ) {

		cero();

		return this;

	}

	multiplyScalar( 1 / d );

	return this;

}

scalar Vector3::normalizeAndModulus() {

	scalar d = length();

	if ( fabs( d ) < 0.00000001 ) {

		cero();

		return 0.0;

	}

	multiplyScalar( 1 / d );

	return d;

}

Vector3 *Vector3::lerp( Vector3 *a, Vector3 *b, scalar f ) {

	scalar f1 = 1.0 - f;

	this->x = a->x * f1 + b->x * f;
	this->y = a->y * f1 + b->y * f;
	this->z = a->z * f1 + b->z * f;

	return this;

}

Vector3 *Vector3::clamp( Vector3 *min, Vector3 *max ) {


	this->x = minim( max->x, maxim( min->x , this->x ) );
	this->y = minim( max->y, maxim( min->y , this->y ) );
	this->z = minim( max->z, maxim( min->z , this->z ) );

	return this;

}

Vector3 *Vector3::copyToArray( scalar *dest, int32_t offset ) {

	dest[ offset ] = x;
	dest[ offset + 1 ] = y;
	dest[ offset + 2 ] = z;

	return this;

}

Vector3 *Vector3::copyFromArray( scalar *source, int32_t offset ) {

	x = source[ offset ];
	y = source[ offset + 1 ];
	z = source[ offset + 2 ];

	return this;

}

std::string Vector3::toString() {

	return std::string( "[ " ) + std::to_string( x ) + ", " + std::to_string( y ) + ", " + std::to_string( z ) + "]";

}

scalar Vector3::maxDimension() {

	return maxim( maxim( x, y ), z );

}

Vector3 *Vector3::reflect( Vector3 *incident, Vector3 *normal ) {

	*this = normal->multiplyScalar( 2.0 * normal->dotProduct( incident ) );
	this->dec( incident );
	return this;

}
