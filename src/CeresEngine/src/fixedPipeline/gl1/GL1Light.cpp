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

#include "GL1Light.h"

#include <GL/gl.h>

#include "console/console.h"

using namespace ceres;

GL1Light::GL1Light() : FPLight() {
}

GL1Light::~GL1Light() {
}

void GL1Light::updateParams() {

	if ( ! enabled ) {

		glDisable( GL_LIGHT0 + lightIndex );
		return;
	}

	glEnable( GL_LIGHT0 + lightIndex );

	float rgba[ 4 ];
	ambient.toArray( rgba, true );
	glLightfv( GL_LIGHT0 + lightIndex, GL_AMBIENT, rgba );

	diffuse.toArray( rgba, true );
	glLightfv( GL_LIGHT0 + lightIndex, GL_DIFFUSE, rgba );

	//specular.toArray( rgba, true );
	//glLightfv( GL_LIGHT0 + lightIndex, GL_SPECULAR, rgba );

}

void GL1Light::updatePosition( Camera *camera ) {

	if ( ! enabled ) return;

	Vector3 p = camera->poseMatrixInverse.vector3XMatrix4( &position );
	float xyzw[ 4 ];
	p.copyToArray( xyzw );
	xyzw[ 3 ] = isSpot ? 1.0 : 0.0;
	glLightfv( GL_LIGHT0 + lightIndex, GL_POSITION, xyzw );

	if ( isSpot ) {
		Vector3 spot = camera->poseMatrixInverse.vector3XMatrix4( &spotDirection );
		spot.copyToArray( xyzw );
		glLightfv(GL_LIGHT0 +  lightIndex, GL_SPOT_DIRECTION, xyzw );
	}

}
