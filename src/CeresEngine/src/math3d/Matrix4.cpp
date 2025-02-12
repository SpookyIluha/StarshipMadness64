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

#include "Matrix4.h"

#include <math.h>
#include <stdio.h>	// debug, remove

using namespace ceres;

Matrix4::Matrix4() {

	this->identity();

}

Matrix4::Matrix4( Matrix4 *m ) {
	copy( m );
}

Matrix4::~Matrix4() {}

Matrix4 *Matrix4::copy( Matrix4 *m ) {

	scalar *es = m->e;

	e[ 0 ] = es[ 0 ];
	e[ 1 ] = es[ 1 ];
	e[ 2 ] = es[ 2 ];
	e[ 3 ] = es[ 3 ];
	e[ 4 ] = es[ 4 ];
	e[ 5 ] = es[ 5 ];
	e[ 6 ] = es[ 6 ];
	e[ 7 ] = es[ 7 ];
	e[ 8 ] = es[ 8 ];
	e[ 9 ] = es[ 9 ];
	e[ 10 ] = es[ 10 ];
	e[ 11 ] = es[ 11 ];
	e[ 12 ] = es[ 12 ];
	e[ 13 ] = es[ 13 ];
	e[ 14 ] = es[ 14 ];
	e[ 15 ] = es[ 15 ];

	return this;
}

Matrix4 *Matrix4::cero() {

	e[ 0 ] = 0.0;
	e[ 1 ] = 0.0;
	e[ 2 ] = 0.0;
	e[ 3 ] = 0.0;
	e[ 4 ] = 0.0;
	e[ 5 ] = 0.0;
	e[ 6 ] = 0.0;
	e[ 7 ] = 0.0;
	e[ 8 ] = 0.0;
	e[ 9 ] = 0.0;
	e[ 10 ] = 0.0;
	e[ 11 ] = 0.0;
	e[ 12 ] = 0.0;
	e[ 13 ] = 0.0;
	e[ 14 ] = 0.0;
	e[ 15 ] = 0.0;

	return this;

}

Matrix4 *Matrix4::identity() {

	e[ 0 ] = 1.0;
	e[ 1 ] = 0.0;
	e[ 2 ] = 0.0;
	e[ 3 ] = 0.0;
	e[ 4 ] = 0.0;
	e[ 5 ] = 1.0;
	e[ 6 ] = 0.0;
	e[ 7 ] = 0.0;
	e[ 8 ] = 0.0;
	e[ 9 ] = 0.0;
	e[ 10 ] = 1.0;
	e[ 11 ] = 0.0;
	e[ 12 ] = 0.0;
	e[ 13 ] = 0.0;
	e[ 14 ] = 0.0;
	e[ 15 ] = 1.0;

	return this;

}

Matrix4 *Matrix4::multiply( Matrix4 * m1, Matrix4 *m2 ) {

	scalar *dest = e;
	scalar *e1 = m1->e;
	scalar *e2 = m2->e;

	dest[ M00 ] = e1[ M00 ] * e2[ M00 ] + e1[ M10 ] * e2[ M01 ] + e1[ M20 ] * e2[ M02 ] + e1[ M30 ] * e2[ M03 ];
	dest[ M10 ] = e1[ M00 ] * e2[ M10 ] + e1[ M10 ] * e2[ M11 ] + e1[ M20 ] * e2[ M12 ] + e1[ M30 ] * e2[ M13 ];
	dest[ M20 ] = e1[ M00 ] * e2[ M20 ] + e1[ M10 ] * e2[ M21 ] + e1[ M20 ] * e2[ M22 ] + e1[ M30 ] * e2[ M23 ];
	dest[ M30 ] = e1[ M00 ] * e2[ M30 ] + e1[ M10 ] * e2[ M31 ] + e1[ M20 ] * e2[ M32 ] + e1[ M30 ] * e2[ M33 ];

	dest[ M01 ] = e1[ M01 ] * e2[ M00 ] + e1[ M11 ] * e2[ M01 ] + e1[ M21 ] * e2[ M02 ] + e1[ M31 ] * e2[ M03 ];
	dest[ M11 ] = e1[ M01 ] * e2[ M10 ] + e1[ M11 ] * e2[ M11 ] + e1[ M21 ] * e2[ M12 ] + e1[ M31 ] * e2[ M13 ];
	dest[ M21 ] = e1[ M01 ] * e2[ M20 ] + e1[ M11 ] * e2[ M21 ] + e1[ M21 ] * e2[ M22 ] + e1[ M31 ] * e2[ M23 ];
	dest[ M31 ] = e1[ M01 ] * e2[ M30 ] + e1[ M11 ] * e2[ M31 ] + e1[ M21 ] * e2[ M32 ] + e1[ M31 ] * e2[ M33 ];

	dest[ M02 ] = e1[ M02 ] * e2[ M00 ] + e1[ M12 ] * e2[ M01 ] + e1[ M22 ] * e2[ M02 ] + e1[ M32 ] * e2[ M03 ];
	dest[ M12 ] = e1[ M02 ] * e2[ M10 ] + e1[ M12 ] * e2[ M11 ] + e1[ M22 ] * e2[ M12 ] + e1[ M32 ] * e2[ M13 ];
	dest[ M22 ] = e1[ M02 ] * e2[ M20 ] + e1[ M12 ] * e2[ M21 ] + e1[ M22 ] * e2[ M22 ] + e1[ M32 ] * e2[ M23 ];
	dest[ M32 ] = e1[ M02 ] * e2[ M30 ] + e1[ M12 ] * e2[ M31 ] + e1[ M22 ] * e2[ M32 ] + e1[ M32 ] * e2[ M33 ];

	dest[ M03 ] = e1[ M03 ] * e2[ M00 ] + e1[ M13 ] * e2[ M01 ] + e1[ M23 ] * e2[ M02 ] + e1[ M33 ] * e2[ M03 ];
	dest[ M13 ] = e1[ M03 ] * e2[ M10 ] + e1[ M13 ] * e2[ M11 ] + e1[ M23 ] * e2[ M12 ] + e1[ M33 ] * e2[ M13 ];
	dest[ M23 ] = e1[ M03 ] * e2[ M20 ] + e1[ M13 ] * e2[ M21 ] + e1[ M23 ] * e2[ M22 ] + e1[ M33 ] * e2[ M23 ];
	dest[ M33 ] = e1[ M03 ] * e2[ M30 ] + e1[ M13 ] * e2[ M31 ] + e1[ M23 ] * e2[ M32 ] + e1[ M33 ] * e2[ M33 ];


	/* Code not unrolled:
	for ( int32_t i = 0; i < 4; i++ ) {
		for ( int32_t j = 0; j < 4; j++ ) {
			int ind = j + i * 4;
			dest[ ind ] = 0.0;
			for ( int32_t k = 0; k < 4; k++ ) {
				dest[ ind ] += e1[ j + k * 4 ] * e2[ k + i * 4 ];
			}
		}
	}
	*/

	return this;

}

Matrix4 *Matrix4::transpose3x3( Matrix4 *m ) {

	scalar *orig = m->e;

	e[ M00 ] = orig[ M00 ];
	e[ M01 ] = orig[ M10 ];
	e[ M02 ] = orig[ M20 ];
	e[ M03 ] = orig[ M03 ];

	e[ M10 ] = orig[ M01 ];
	e[ M11 ] = orig[ M11 ];
	e[ M12 ] = orig[ M21 ];
	e[ M13 ] = orig[ M13 ];

	e[ M20 ] = orig[ M02 ];
	e[ M21 ] = orig[ M12 ];
	e[ M22 ] = orig[ M22 ];
	e[ M23 ] = orig[ M23 ];

	e[ M30 ] = orig[ M30 ];
	e[ M31 ] = orig[ M31 ];
	e[ M32 ] = orig[ M32 ];
	e[ M33 ] = orig[ M33 ];

	return this;

}

Matrix4 *Matrix4::transpose4x4( Matrix4 *m ) {

	scalar *orig = m->e;

	e[ M00 ] = orig[ M00 ];
	e[ M01 ] = orig[ M10 ];
	e[ M02 ] = orig[ M20 ];
	e[ M03 ] = orig[ M30 ];

	e[ M10 ] = orig[ M01 ];
	e[ M11 ] = orig[ M11 ];
	e[ M12 ] = orig[ M21 ];
	e[ M13 ] = orig[ M31 ];

	e[ M20 ] = orig[ M02 ];
	e[ M21 ] = orig[ M12 ];
	e[ M22 ] = orig[ M22 ];
	e[ M23 ] = orig[ M32 ];

	e[ M30 ] = orig[ M03 ];
	e[ M31 ] = orig[ M13 ];
	e[ M32 ] = orig[ M23 ];
	e[ M33 ] = orig[ M33 ];

	return this;

}

Matrix4 *Matrix4::inverse( Matrix4 *m ) {

	/*

	Based on MIT Licensed Threejs https://github.com/mrdoob/three.js/blob/dev/src/math/Matrix4.js
	based on http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm

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

	scalar *te = this->e;
	scalar *me = m->e;

	scalar
	n11 = me[ 0 ], n21 = me[ 1 ], n31 = me[ 2 ], n41 = me[ 3 ],
	n12 = me[ 4 ], n22 = me[ 5 ], n32 = me[ 6 ], n42 = me[ 7 ],
	n13 = me[ 8 ], n23 = me[ 9 ], n33 = me[ 10 ], n43 = me[ 11 ],
	n14 = me[ 12 ], n24 = me[ 13 ], n34 = me[ 14 ], n44 = me[ 15 ],

	t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44,
	t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44,
	t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44,
	t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34,

	det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;

	if ( det == 0.0f ) return this->cero();

	scalar detInv = 1.0 / det;

	te[ 0 ] = t11 * detInv;
	te[ 1 ] = ( n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44 ) * detInv;
	te[ 2 ] = ( n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44 ) * detInv;
	te[ 3 ] = ( n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43 ) * detInv;

	te[ 4 ] = t12 * detInv;
	te[ 5 ] = ( n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44 ) * detInv;
	te[ 6 ] = ( n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44 ) * detInv;
	te[ 7 ] = ( n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43 ) * detInv;

	te[ 8 ] = t13 * detInv;
	te[ 9 ] = ( n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44 ) * detInv;
	te[ 10 ] = ( n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44 ) * detInv;
	te[ 11 ] = ( n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43 ) * detInv;

	te[ 12 ] = t14 * detInv;
	te[ 13 ] = ( n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34 ) * detInv;
	te[ 14 ] = ( n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34 ) * detInv;
	te[ 15 ] = ( n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33 ) * detInv;

	return this;

}

Matrix4 *Matrix4::inverseHomogeneous( Matrix4 *m ) {

	// Only for homogeneous transforms

	// Inverts 3x3 submatrix
	transpose3x3( m );

	// Inverts the translation
	Vector3 v1 = m->getTraslation();
	v1.negate();

	Vector3 v2 = vector3FreeXMatrix4( &v1 );

	// And put it back in
	setTraslation( &v2 );

	return this;

}

Matrix4 *Matrix4::relative( Matrix4 *m1, Matrix4 *m2 ) {

	// Computes the relative matrix between m1 and m2, that is, the transform to get from m1 space to m2 space

	Matrix4 m1Inverse;
	m1Inverse.inverseHomogeneous( m1 );

	Vector3 p2 = m2->getTraslation();
	Vector3 x2 = m2->getDirectorVectorX();
	Vector3 y2 = m2->getDirectorVectorY();
	Vector3 z2 = m2->getDirectorVectorZ();

	Vector3 p = m1->getTraslation();

	Vector3 pr = p2.dec( &p );
	Vector3 xr = m1Inverse.vector3FreeXMatrix4( &x2 );
	Vector3 yr = m1Inverse.vector3FreeXMatrix4( &y2 );
	Vector3 zr = m1Inverse.vector3FreeXMatrix4( &z2 );

	setDirectorVectors( &xr, &yr, &zr );
	setTraslation( &pr );

	return this;

}

Matrix4 *Matrix4::setTraslation( scalar x, scalar y, scalar z ) {

	e[ M30 ] = x;
	e[ M31 ] = y;
	e[ M32 ] = z;

	return this;

}

Matrix4 *Matrix4::setTraslation( Vector3 *v ) {

	e[ M30 ] = v->x;
	e[ M31 ] = v->y;
	e[ M32 ] = v->z;

	return this;

}

Matrix4 *Matrix4::addTraslation( scalar x, scalar y, scalar z ) {

	e[ M30 ] += x;
	e[ M31 ] += y;
	e[ M32 ] += z;

	return this;

}

Matrix4 *Matrix4::addTraslation( Vector3 *v ) {

	e[ M30 ] += v->x;
	e[ M31 ] += v->y;
	e[ M32 ] += v->z;

	return this;

}

Vector3 Matrix4::getTraslation() {

	return Vector3( e[ M30 ], e[ M31 ], e[ M32 ] );

}

Vector3 Matrix4::getDirectorVectorX() {

	return Vector3( e[ M00 ], e[ M01 ], e[ M02 ] );

}

Vector3 Matrix4::getDirectorVectorY() {

	return Vector3( e[ M10 ], e[ M11 ], e[ M12 ] );

}

Vector3 Matrix4::getDirectorVectorZ() {

	return Vector3( e[ M20 ], e[ M21 ], e[ M22 ] );

}

Matrix4 *Matrix4::setDirectorVectors( Vector3 *axisX, Vector3 *axisY, Vector3 *axisZ ) {

	e[ M00 ] = axisX->x;
	e[ M01 ] = axisX->y;
	e[ M02 ] = axisX->z;
	e[ M10 ] = axisY->x;
	e[ M11 ] = axisY->y;
	e[ M12 ] = axisY->z;
	e[ M20 ] = axisZ->x;
	e[ M21 ] = axisZ->y;
	e[ M22 ] = axisZ->z;

	return this;

}

Matrix4 *Matrix4::setRotationFromQuaternion( Quaternion *q ) {

	double x2 = q->x + q->x;
	double y2 = q->y + q->y;
	double z2 = q->z + q->z;
	double xx = q->x * x2;
	double xy = q->x * y2;
	double xz = q->x * z2;
	double yy = q->y * y2;
	double yz = q->y * z2;
	double zz = q->z * z2;
	double wx = q->w * x2;
	double wy = q->w * y2;
	double wz = q->w * z2;

	e[ M00 ] = 1.0 - ( yy + zz );
	e[ M10 ] = xy - wz;
	e[ M20 ] = xz + wy;

	e[ M01 ] = xy + wz;
	e[ M11 ] = 1.0 - ( xx + zz );
	e[ M21 ] = yz - wx;

	e[ M02 ] = xz - wy;
	e[ M12 ] = yz + wx;
	e[ M22 ] = 1.0 - ( xx + yy );

	return this;

}

Matrix4 *Matrix4::scale( Vector3 *s ) {

	e[ M00 ] *= s->x;
	e[ M01 ] *= s->x;
	e[ M02 ] *= s->x;
	e[ M03 ] *= s->x;

	e[ M10 ] *= s->y;
	e[ M11 ] *= s->y;
	e[ M12 ] *= s->y;
	e[ M13 ] *= s->y;

	e[ M20 ] *= s->z;
	e[ M21 ] *= s->z;
	e[ M22 ] *= s->z;
	e[ M23 ] *= s->z;


	return this;

}

Vector3 Matrix4::vector3XMatrix4( Vector3 *v ) {

	return Vector3(
			e[ M00 ] * v->x +
			e[ M10 ] * v->y +
			e[ M20 ] * v->z +
			e[ M30 ],

			e[ M01 ] * v->x +
			e[ M11 ] * v->y +
			e[ M21 ] * v->z +
			e[ M31 ],

			e[ M02 ] * v->x +
			e[ M12 ] * v->y +
			e[ M22 ] * v->z +
			e[ M32 ]

	);

}

Vector3 Matrix4::vector3FreeXMatrix4( Vector3 *v ) {

	return Vector3(
			e[ M00 ] * v->x +
			e[ M10 ] * v->y +
			e[ M20 ] * v->z,

			e[ M01 ] * v->x +
			e[ M11 ] * v->y +
			e[ M21 ] * v->z,

			e[ M02 ] * v->x +
			e[ M12 ] * v->y +
			e[ M22 ] * v->z
	);

}

Vector3 Matrix4::vector3FreeXMatrix4Inverse( Vector3 *v ) {

	return Vector3(
			e[ M00 ] * v->x +
			e[ M01 ] * v->y +
			e[ M02 ] * v->z,

			e[ M10 ] * v->x +
			e[ M11 ] * v->y +
			e[ M12 ] * v->z,

			e[ M20 ] * v->x +
			e[ M21 ] * v->y +
			e[ M22 ] * v->z
	);

}

Vector4 Matrix4::vector4XMatrix4( Vector3 *v ) {

	return Vector4(
			e[ M00 ] * v->x +
			e[ M10 ] * v->y +
			e[ M20 ] * v->z +
			e[ M30 ],

			e[ M01 ] * v->x +
			e[ M11 ] * v->y +
			e[ M21 ] * v->z +
			e[ M31 ],

			e[ M02 ] * v->x +
			e[ M12 ] * v->y +
			e[ M22 ] * v->z +
			e[ M32 ],

			e[ M03 ] * v->x +
			e[ M13 ] * v->y +
			e[ M23 ] * v->z +
			e[ M33 ]

	);

}

Vector4 Matrix4::vector4XMatrix4( Vector4 *v ) {

	return Vector4(
			e[ M00 ] * v->x +
			e[ M10 ] * v->y +
			e[ M20 ] * v->z +
			e[ M30 ] * v->w,

			e[ M01 ] * v->x +
			e[ M11 ] * v->y +
			e[ M21 ] * v->z +
			e[ M31 ] * v->w,

			e[ M02 ] * v->x +
			e[ M12 ] * v->y +
			e[ M22 ] * v->z +
			e[ M32 ] * v->w,

			e[ M03 ] * v->x +
			e[ M13 ] * v->y +
			e[ M23 ] * v->z +
			e[ M33 ] * v->w

	);

}

Matrix4 *Matrix4::orthogonalProjection() {

	// TODO

	return this;

}

Matrix4 *Matrix4::conicProjection( scalar fovYDegrees, scalar aspect, scalar minZ, scalar maxZ ) {

	double angle = fovYDegrees * 0.5 * M_PI / 180.0;
	double cot = cos( angle ) / sin( angle );

	e[ 0 ] = cot / aspect;
	e[ 1 ] = 0.0;
	e[ 2 ] = 0.0;
	e[ 3 ] = 0.0;
	e[ 4 ] = 0.0;
	e[ 5 ] = cot;
	e[ 6 ] = 0.0;
	e[ 7 ] = 0.0;
	e[ 8 ] = 0.0;
	e[ 9 ] = 0.0;
	e[ 10 ] = -( maxZ + minZ ) / ( maxZ - minZ );
	e[ 11 ] = -1.0;
	e[ 12 ] = 0.0;
	e[ 13 ] = 0.0;
	e[ 14 ] = -( 2.0 * maxZ * minZ ) / ( maxZ - minZ );
	e[ 15 ] = 0.0;

	return this;

}

Matrix4 *Matrix4::lookAt( Vector3 *traslation, Vector3 *target, Vector3 *up ) {

	// This function does not set the matrix traslation

	Vector3 axisZ;
	axisZ.sub( target, traslation );
	if ( axisZ.normalizeAndModulus() < 0.00000001 ) return identity();

	if ( up->normalizeAndModulus() < 0.00000001 ) return identity();

	Vector3 axisX;
	axisX.crossProduct( &axisZ, up );
	if ( axisX.normalizeAndModulus() < 0.00000001 ) return identity();

	up->crossProduct( &axisX, &axisZ );

	axisZ.negate();

	setDirectorVectors( &axisX, up, &axisZ );

	return this;

}

Matrix4 *Matrix4::rotationYXZ( scalar angX, scalar angY, scalar angZ ) {

	// Leaves traslation unmodified

	scalar sx = sin( angX );
	scalar sy = sin( angY );
	scalar sz = sin( angZ );
	scalar cx = cos( angX );
	scalar cy = cos( angY );
	scalar cz = cos( angZ );

	e[ M00 ] = cz * cy + sz * sx * sy;
	e[ M10 ] = - sz * cy + cz * sx * sy;
	e[ M20 ] = cx * sy;

	e[ M01 ] = sz * cx;
	e[ M11 ] = cz * cx;
	e[ M21 ] = - sx;

	e[ M02 ] = - cz * sy + sz * sx * cy;
	e[ M12 ] = sz * sy + cz * sx * cy;
	e[ M22 ] = cx * cy;

	return this;

}

Matrix4 *Matrix4::rotationYX( scalar angX, scalar angY ) {

	// Leaves traslation unmodified

	scalar sx = sin( angX );
	scalar sy = sin( angY );
	scalar cx = cos( angX );
	scalar cy = cos( angY );

	e[ M00 ] = cy;
	e[ M10 ] = sx * sy;
	e[ M20 ] = cx * sy;

	e[ M01 ] = 0.0;
	e[ M11 ] = cx;
	e[ M21 ] = - sx;

	e[ M02 ] = - sy;
	e[ M12 ] = sx * cy;
	e[ M22 ] = cx * cy;

	return this;

}

Matrix4 *Matrix4::rotationXZ( scalar angX, scalar angZ ) {

	// Leaves traslation unmodified

	scalar sx = sin( angX );
	scalar sz = sin( angZ );
	scalar cx = cos( angX );
	scalar cz = cos( angZ );

	e[ M00 ] = cz;
	e[ M10 ] = - sz;
	e[ M20 ] = 0.0;

	e[ M01 ] = sz * cx;
	e[ M11 ] = cz * cx;
	e[ M21 ] = - sx;

	e[ M02 ] = sz * sx;
	e[ M12 ] = cz * sx;
	e[ M22 ] = cx;

	return this;

}

Matrix4 *Matrix4::copyToArray( scalar *dest ) {

	dest[ 0 ] = e[ 0 ];
	dest[ 1 ] = e[ 1 ];
	dest[ 2 ] = e[ 2 ];
	dest[ 3 ] = e[ 3 ];
	dest[ 4 ] = e[ 4 ];
	dest[ 5 ] = e[ 5 ];
	dest[ 6 ] = e[ 6 ];
	dest[ 7 ] = e[ 7 ];
	dest[ 8 ] = e[ 8 ];
	dest[ 9 ] = e[ 9 ];
	dest[ 10 ] = e[ 10 ];
	dest[ 11 ] = e[ 11 ];
	dest[ 12 ] = e[ 12 ];
	dest[ 13 ] = e[ 13 ];
	dest[ 14 ] = e[ 14 ];
	dest[ 15 ] = e[ 15 ];

	return this;

}

Matrix4 *Matrix4::copyFromArray( scalar *orig ) {

	e[ 0 ] = orig[ 0 ];
	e[ 1 ] = orig[ 1 ];
	e[ 2 ] = orig[ 2 ];
	e[ 3 ] = orig[ 3 ];
	e[ 4 ] = orig[ 4 ];
	e[ 5 ] = orig[ 5 ];
	e[ 6 ] = orig[ 6 ];
	e[ 7 ] = orig[ 7 ];
	e[ 8 ] = orig[ 8 ];
	e[ 9 ] = orig[ 9 ];
	e[ 10 ] = orig[ 10 ];
	e[ 11 ] = orig[ 11 ];
	e[ 12 ] = orig[ 12 ];
	e[ 13 ] = orig[ 13 ];
	e[ 14 ] = orig[ 14 ];
	e[ 15 ] = orig[ 15 ];

	return this;

}

std::string Matrix4::toString() {

	return std::string( "[ " ) +
		std::to_string( e[ 0 ] ) + "\n" +
		std::to_string( e[ 1 ] ) + "\n" +
		std::to_string( e[ 2 ] ) + "\n" +
		std::to_string( e[ 3 ] ) + "\n" +
		std::to_string( e[ 4 ] ) + "\n" +
		std::to_string( e[ 5 ] ) + "\n" +
		std::to_string( e[ 6 ] ) + "\n" +
		std::to_string( e[ 7 ] ) + "\n" +
		std::to_string( e[ 8 ] ) + "\n" +
		std::to_string( e[ 9 ] ) + "\n" +
		std::to_string( e[ 10 ] ) + "\n" +
		std::to_string( e[ 11 ] ) + "\n" +
		std::to_string( e[ 12 ] ) + "\n" +
		std::to_string( e[ 13 ] ) + "\n" +
		std::to_string( e[ 14 ] ) + "\n" +
		std::to_string( e[ 15 ] ) + "]";

}
