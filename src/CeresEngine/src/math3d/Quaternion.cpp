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

#include "Quaternion.h"

#include <math.h>

using namespace ceres;

Quaternion::Quaternion(): Quaternion( 0.0, 0.0, 0.0, 1.0 ) {}

Quaternion::Quaternion( scalar x, scalar y, scalar z, scalar w ) {

	this->set( x, y, z, w );

}

Quaternion *Quaternion::identity() {
	return set( 0.0, 0.0, 0.0, 1.0 );
}

Quaternion* Quaternion::set( scalar x, scalar y, scalar z, scalar w ) {

	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;

	return this;

}

Quaternion *Quaternion::copy( Quaternion *source ) {

	x = source->x;
	y = source->y;
	z = source->z;
	w = source->w;

	return this;

}

Quaternion *Quaternion::copyToArray( scalar *dest, int32_t offset ) {

	dest[ offset ] = x;
	dest[ offset + 1 ] = y;
	dest[ offset + 2 ] = z;
	dest[ offset + 3 ] = w;

	return this;
}

Quaternion *Quaternion::copyFromArray( scalar *source, int32_t offset ) {

	x = source[ offset ];
	y = source[ offset + 1 ];
	z = source[ offset + 2 ];
	w = source[ offset + 3 ];

	return this;

}


Quaternion::~Quaternion() {}

std::string Quaternion::toString() {

	return std::string( "[ " ) + std::to_string( x ) + ", " + std::to_string( y ) + ", " + std::to_string( z ) + ", " + std::to_string( w ) + "]";

}


Quaternion* Quaternion::setFromAxisAngle( Vector3 *axis, scalar angle ) {

	/*

	Based on MIT Licensed Threejs threejs.org
	http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm

	The MIT License

	Copyright © 2010-2024 three.js authors

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	*/

	// assumes axis is normalized

	const scalar halfAngle = angle * 0.5;
	const scalar s = sin( halfAngle );

	x = axis->x * s;
	y = axis->y * s;
	z = axis->z * s;
	w = cos( halfAngle );

	return this;

}

Quaternion* Quaternion::multiplyQuaternions( Quaternion *a,Quaternion *b ) {

	// From https://threejs.org
	// From http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm

	const scalar qax = a->x, qay = a->y, qaz = a->z, qaw = a->w;
	const scalar qbx = b->x, qby = b->y, qbz = b->z, qbw = b->w;

	x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
	y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
	z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
	w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

	return this;

}

Quaternion* Quaternion::interpolateQuaternions( Quaternion *qFrom,Quaternion *qTo, scalar t ) {

	scalar cosom = qFrom->x * qTo->x + qFrom->y * qTo->y + qFrom->z * qTo->z + qFrom->w * qTo->w;

	Quaternion q = *qTo;
	if ( cosom < 0 ) {
		cosom = - cosom;
		q.x = - q.x;
		q.y = - q.y;
		q.z = - q.z;
		q.w = - q.w;
	}

	scalar omega = 0.0;
	scalar sinom = 0.0;
	scalar scale0 = 0.0;
	scalar scale1 = 0.0;
	if ( 1.0 - cosom > 0.001 ) {
		// Slerp
		omega = acos( cosom );
		sinom = sin( omega );
		scale0 = sin( ( 1.0 - t ) * omega ) / sinom;
		scale1 = sin( t * omega ) / sinom;
	}
	else {
		// Linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}

	x = scale0 * qFrom->x + scale1 * q.x;
	y = scale0 * qFrom->y + scale1 * q.y;
	z = scale0 * qFrom->z + scale1 * q.z ;
	w = scale0 * qFrom->w + scale1 * q.w;

	return this;

}
