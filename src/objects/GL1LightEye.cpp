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

#include "GL1LightEye.h"

#include <GL/gl.h>

#include "console/console.h"

using namespace ceres;

GL1LightEye::GL1LightEye() : GL1Light() {
}

GL1LightEye::~GL1LightEye() {
}

void GL1LightEye::updatePosition( Camera *camera ) {

	if ( ! enabled ) return;

	float xyzw[ 4 ];
	position.copyToArray( xyzw );
	xyzw[ 3 ] = isSpot ? 1.0 : 0.0;

	glLightfv( GL_LIGHT0 + lightIndex, GL_POSITION, xyzw );

	if ( isSpot ) {
		spotDirection.copyToArray( xyzw );
		glLightfv(GL_LIGHT0 +  lightIndex, GL_SPOT_DIRECTION, xyzw );
	}


}
