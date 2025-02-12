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

#include "MathUtils.h"

using namespace ceres;

void ceres::frandseed( int32_t x ) {
	srand( x );
}
void ceres::frandseedtime() {
	( srand( static_cast <unsigned> ( 0 ) ) );
}

scalar ceres::frand() {
	return ( static_cast <scalar> ( rand() ) / static_cast <scalar> ( RAND_MAX ) );
}

scalar ceres::minim( scalar a, scalar b ) {
	return ( ( a ) < ( b ) ? ( a ) : ( b ) );
}
scalar ceres::maxim( scalar a, scalar b ) {
	return ( ( a ) > ( b ) ? ( a ) : ( b ) );
}

scalar ceres::deg2rad( scalar deg ) {

	return deg * pi / 180.0;

}

scalar ceres::rad2deg( scalar rad ) {

	return rad * 180.0 / pi;

}
