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

#include "Vector2.h"

#include <math.h>

using namespace ceres;

Vector2::Vector2() : Vector2( 0.0, 0.0 ) {}

Vector2::Vector2( Vector2 *a ) {

	set( a->x, a->y );

}

Vector2::Vector2( scalar x, scalar y ) {

	set( x, y );

}

Vector2::Vector2( scalar xy ) {

	set( xy, xy );

}

Vector2::~Vector2() {}


Vector2 *Vector2::copy( Vector2 *src ) {

	return set( src->x, src->y );

}

Vector2 *Vector2::set( scalar x, scalar y ) {

	this->x = x;
	this->y = y;

	return this;

}

Vector2 *Vector2::cero() {

	set( 0.0, 0.0 );

	return this;
}

bool Vector2::isCero() {

	return ( x == 0.0 ) && ( y == 0.0 );

}

Vector2 *Vector2::inc( Vector2 *a ) {

	x += a->x;
	y += a->y;

	return this;

}

Vector2 *Vector2::inc( scalar x, scalar y ) {

	this->x += x;
	this->y += y;

	return this;

}

Vector2 *Vector2::dec( Vector2 *a ) {

	x -= a->x;
	y -= a->y;

	return this;

}

Vector2 *Vector2::dec( scalar x, scalar y ) {

	this->x -= x;
	this->y -= y;

	return this;

}

Vector2 *Vector2::add( Vector2 *a, Vector2 *b ) {

	x = a->x + b->x;
	y = a->y + b->y;

	return this;
}

Vector2 *Vector2::sub( Vector2 *a, Vector2 *b ) {

	x = a->x - b->x;
	y = a->y - b->y;

	return this;

}

Vector2 *Vector2::negate() {

	x = - x;
	y = - y;

	return this;

}

Vector2 *Vector2::multiplyScalar( scalar s ) {

	x *= s;
	y *= s;

	return this;

}

scalar Vector2::dotProduct( Vector2 *b ) {

	return x * b->x + y * b->y;

}

scalar Vector2::length() {

	return sqrt( x * x + y * y );

}

scalar Vector2::lengthSquared() {

	return x * x + y * y;

}

scalar Vector2::distanceTo( Vector2 *a ) {

	Vector2 d;
	d.sub( this, a );
	return d.length();

}

Vector2 *Vector2::normalize() {

	scalar d = length();

	if ( fabs( d ) < 0.00000001 ) {

		cero();

		return this;

	}

	multiplyScalar( 1 / d );

	return this;

}

scalar Vector2::normalizeAndModulus() {

	scalar d = length();

	if ( fabs( d ) < 0.00000001 ) {

		cero();

		return 0.0;

	}

	multiplyScalar( 1 / d );

	return d;

}

Vector2 *Vector2::lerp( Vector2 *a, Vector2 *b, scalar f ) {

	scalar f1 = 1.0 - f;

	this->x = a->x * f + b->x * f1;
	this->y = a->y * f + b->y * f1;

	return this;

}

Vector2 *Vector2::clamp( Vector2 *min, Vector2 *max ) {


	this->x = minim( max->x, maxim( min->x, this->x ) );
	this->y = minim( max->y, maxim( min->y, this->y ) );

	return this;

}

Vector2 *Vector2::copyToArray( scalar *dest, int32_t offset ) {

	dest[ offset ] = x;
	dest[ offset + 1 ] = y;

	return this;

}

Vector2 *Vector2::copyFromArray( scalar *source, int32_t offset ) {

	x = source[ offset ];
	y = source[ offset + 1 ];

	return this;

}

std::string Vector2::toString() {

	return std::string( "[ " ) + std::to_string( x ) + ", " + std::to_string( y ) + "]";

}

scalar Vector2::maxDimension() {

	return x > y ? x : y;

}
