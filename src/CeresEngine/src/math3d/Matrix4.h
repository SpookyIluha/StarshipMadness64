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

#include <string>

#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ceres {

class Matrix4 {
public:

	Matrix4();
	Matrix4( Matrix4 *m );
	~Matrix4();

	Matrix4 *copy( Matrix4 *m );
	Matrix4 *cero();
	Matrix4 *identity();
	Matrix4 *multiply( Matrix4 *m1, Matrix4 *m2 );
	Matrix4 *transpose3x3( Matrix4 *m );
	Matrix4 *transpose4x4( Matrix4 *m );
	Matrix4 *inverse( Matrix4 *m );
	Matrix4 *inverseHomogeneous( Matrix4 *m );
	Matrix4 *relative( Matrix4 *m1, Matrix4 *m2 );

	Matrix4 *setTraslation( scalar x, scalar y, scalar z );
	Matrix4 *setTraslation( Vector3 *v );
	Matrix4 *addTraslation( scalar x, scalar y, scalar z );
	Matrix4 *addTraslation( Vector3 *v );

	Vector3 getTraslation();
	Vector3 getDirectorVectorX();
	Vector3 getDirectorVectorY();
	Vector3 getDirectorVectorZ();
	Matrix4 *setDirectorVectors( Vector3 *axisX, Vector3 *axisY, Vector3 *axisZ );

	Matrix4 *scale( Vector3 *s );

	Matrix4 *rotationYXZ( scalar angX, scalar angY, scalar angZ );
	Matrix4 *rotationYX( scalar angX, scalar angY );
	Matrix4 *rotationXZ( scalar angX, scalar angZ );

	Matrix4 *setRotationFromQuaternion( Quaternion *q );
	Matrix4 *lookAt( Vector3 *traslation, Vector3 *target, Vector3 *up );

	Vector3 vector3XMatrix4( Vector3 *v );
	Vector3 vector3FreeXMatrix4( Vector3 *v );
	Vector3 vector3FreeXMatrix4Inverse( Vector3 *v );
	Vector4 vector4XMatrix4( Vector3 *v );
	Vector4 vector4XMatrix4( Vector4 *v );

	Matrix4 *orthogonalProjection();
	Matrix4 *conicProjection( scalar fovYDegrees, scalar aspect, scalar minZ, scalar maxZ );

	Matrix4 *copyToArray( scalar *dest );
	Matrix4 *copyFromArray( scalar *orig );

	std::string toString();

	static const int NUM_ELEMENTS_MATRIX4 = 16;

	scalar e[ NUM_ELEMENTS_MATRIX4 ];

    static const int M00 = 0;
    static const int M01 = 1;
    static const int M02 = 2;
    static const int M03 = 3;

    static const int M10 = 4;
    static const int M11 = 5;
    static const int M12 = 6;
    static const int M13 = 7;

    static const int M20 = 8;
    static const int M21 = 9;
    static const int M22 = 10;
    static const int M23 = 11;

    static const int M30 = 12;
    static const int M31 = 13;
    static const int M32 = 14;
    static const int M33 = 15;

private:

};

}
