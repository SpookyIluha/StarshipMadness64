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

#include "Matrix3.h"

#include <math.h>

using namespace ceres;

Matrix3::Matrix3() {

	this->identity();

}

Matrix3::~Matrix3() {}

Matrix3 *Matrix3::cero() {

	e[ 0 ] = 0.0;
	e[ 1 ] = 0.0;
	e[ 2 ] = 0.0;
	e[ 3 ] = 0.0;
	e[ 4 ] = 0.0;
	e[ 5 ] = 0.0;
	e[ 6 ] = 0.0;
	e[ 7 ] = 0.0;
	e[ 8 ] = 0.0;

	return this;

}

Matrix3 *Matrix3::identity() {

	e[ 0 ] = 1.0;
	e[ 1 ] = 0.0;
	e[ 2 ] = 0.0;
	e[ 3 ] = 0.0;
	e[ 4 ] = 1.0;
	e[ 5 ] = 0.0;
	e[ 6 ] = 0.0;
	e[ 7 ] = 0.0;
	e[ 8 ] = 1.0;

	return this;

}

Matrix3 *Matrix3::transpose3x3( Matrix3 *m ) {

	// m: Source matrix
	// this: Destination matrix

	scalar *orig = m->e;

	e[ M00 ] = orig[ M00 ];
	e[ M01 ] = orig[ M10 ];
	e[ M02 ] = orig[ M20 ];

	e[ M10 ] = orig[ M01 ];
	e[ M11 ] = orig[ M11 ];
	e[ M12 ] = orig[ M21 ];

	e[ M20 ] = orig[ M02 ];
	e[ M21 ] = orig[ M12 ];
	e[ M22 ] = orig[ M22 ];

	return this;

}

Matrix3 *Matrix3::transpose2x2( Matrix3 *m ) {

	// m: Source matrix
	// this: Destination matrix

	scalar *orig = m->e;

	e[ M00 ] = orig[ M00 ];
	e[ M01 ] = orig[ M10 ];
	e[ M02 ] = orig[ M02 ];

	e[ M10 ] = orig[ M01 ];
	e[ M11 ] = orig[ M11 ];
	e[ M12 ] = orig[ M12 ];

	e[ M20 ] = orig[ M20 ];
	e[ M21 ] = orig[ M21 ];
	e[ M22 ] = orig[ M22 ];

	return this;

}

Matrix3 *Matrix3::inverse( Matrix3 *m ) {

	/*

	Based on MIT Licensed Threejs https://github.com/mrdoob/three.js/blob/dev/src/math/Matrix3.js

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

	// m: Source matrix
	// this: Destination matrix

	scalar *me = m->e;
	scalar *te = this->e;

	scalar
	n11 = me[ 0 ], n21 = me[ 1 ], n31 = me[ 2 ],
	n12 = me[ 3 ], n22 = me[ 4 ], n32 = me[ 5 ],
	n13 = me[ 6 ], n23 = me[ 7 ], n33 = me[ 8 ],

	t11 = n33 * n22 - n32 * n23,
	t12 = n32 * n13 - n33 * n12,
	t13 = n23 * n12 - n22 * n13,

	det = n11 * t11 + n21 * t12 + n31 * t13;

	if ( det == 0 ) return this->cero();

	scalar detInv = 1.0 / det;

	te[ 0 ] = t11 * detInv;
	te[ 1 ] = ( n31 * n23 - n33 * n21 ) * detInv;
	te[ 2 ] = ( n32 * n21 - n31 * n22 ) * detInv;

	te[ 3 ] = t12 * detInv;
	te[ 4 ] = ( n33 * n11 - n31 * n13 ) * detInv;
	te[ 5 ] = ( n31 * n12 - n32 * n11 ) * detInv;

	te[ 6 ] = t13 * detInv;
	te[ 7 ] = ( n21 * n13 - n23 * n11 ) * detInv;
	te[ 8 ] = ( n22 * n11 - n21 * n12 ) * detInv;

	return this;

}

Matrix3 *Matrix3::inverseHomogeneous( Matrix3 *m ) {

	// Only for homogeneous transforms

	// Inverts 2x2 submatrix
	transpose2x2( m );

	// Inverts the traslation
	//Vector3 v1 = m->getTraslation();
	//v1.negate();

	scalar x = - e[ M20 ];
	scalar y = - e[ M21 ];

	scalar transfx = x * e[ M00 ] + y * e[ M10 ];
	scalar transfy = y * e[ M01 ] + y * e[ M11 ];

	// And put it back in
	e[ M20 ] = transfx;
	e[ M21 ] = transfy;

	return this;

}

Vector3 Matrix3::vector3XMatrix3( Vector3 *v ) {

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

Matrix3 *Matrix3::copyToArray( scalar *dest ) {

	dest[ 0 ] = e[ 0 ];
	dest[ 1 ] = e[ 1 ];
	dest[ 2 ] = e[ 2 ];
	dest[ 3 ] = e[ 3 ];
	dest[ 4 ] = e[ 4 ];
	dest[ 5 ] = e[ 5 ];
	dest[ 6 ] = e[ 6 ];
	dest[ 7 ] = e[ 7 ];
	dest[ 8 ] = e[ 8 ];

	return this;

}

Matrix3 *Matrix3::copyFromArray( scalar *orig ) {

	e[ 0 ] = orig[ 0 ];
	e[ 1 ] = orig[ 1 ];
	e[ 2 ] = orig[ 2 ];
	e[ 3 ] = orig[ 3 ];
	e[ 4 ] = orig[ 4 ];
	e[ 5 ] = orig[ 5 ];
	e[ 6 ] = orig[ 6 ];
	e[ 7 ] = orig[ 7 ];
	e[ 8 ] = orig[ 8 ];

	return this;

}

std::string Matrix3::toString() {

	return std::string( "[ " ) +
		std::to_string( e[ 0 ] ) + ", " +
		std::to_string( e[ 1 ] ) + ", " +
		std::to_string( e[ 2 ] ) + ", " +
		std::to_string( e[ 3 ] ) + ", " +
		std::to_string( e[ 4 ] ) + ", " +
		std::to_string( e[ 5 ] ) + ", " +
		std::to_string( e[ 6 ] ) + ", " +
		std::to_string( e[ 7 ] ) + ", " +
		std::to_string( e[ 8 ] ) + " ]";

}
