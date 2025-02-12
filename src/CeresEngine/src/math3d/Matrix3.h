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

#include "Vector3.h"

namespace ceres {

class Matrix3 {
public:

	Matrix3();
	~Matrix3();

	Matrix3 *cero();
	Matrix3 *identity();
	Matrix3 *transpose2x2( Matrix3 *m );
	Matrix3 *transpose3x3( Matrix3 *m );
	Matrix3 *inverse( Matrix3 *m );
	Matrix3 *inverseHomogeneous( Matrix3 *m );

	Vector3 vector3XMatrix3( Vector3 *v );

	Matrix3 *copyToArray( scalar *dest );
	Matrix3 *copyFromArray( scalar *orig );

	std::string toString();

	static const int NUM_ELEMENTS_MATRIX3 = 9;

	scalar e[ NUM_ELEMENTS_MATRIX3 ];

    static const int M00 = 0;
    static const int M01 = 1;
    static const int M02 = 2;

    static const int M10 = 3;
    static const int M11 = 4;
    static const int M12 = 5;

    static const int M20 = 6;
    static const int M21 = 7;
    static const int M22 = 8;

private:

};

}
