/*
 * Copyright © 2020 - 2024 yomboprime https://codeberg.org/yombo
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

#include "CondensationLines.h"

#include <GL/gl.h>

using namespace ceres;

CondensationLines::CondensationLines() :
	thickness( 0.2 ) {
}

CondensationLines::~CondensationLines(){
}

void CondensationLines::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	Vector3 p0, p1, p2, p3;

	glBegin( GL_TRIANGLES );

	for ( int32_t i = 0; i < NUM_TRAIL_POINTS - 1; i ++ ) {

		Vector3 side;
		side.set( 1, 0, 0 )->multiplyScalar( thickness );

		p0.copy( &trailPoints[ i ] )->dec( &side );
		p1.copy( &trailPoints[ i ] )->inc( &side );
		p2.copy( &trailPoints[ i + 1 ] )->dec( &side );
		p3.copy( &trailPoints[ i + 1 ] )->inc( &side );

		float v[ 12 ];

		v[ 0 ] = p0.x;
		v[ 1 ] = p0.y;
		v[ 2 ] = p0.z;
		v[ 3 ] = p1.x;
		v[ 4 ] = p1.y;
		v[ 5 ] = p1.z;
		v[ 6 ] = p2.x;
		v[ 7 ] = p2.y;
		v[ 8 ] = p2.z;
		v[ 9 ] = p3.x;
		v[ 10 ] = p3.y;
		v[ 11 ] = p3.z;

		drawQuad( v );

	}

	glEnd();

	finishRender();

}

void CondensationLines::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

}

void CondensationLines::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}

void CondensationLines::drawQuad( float *v ) {

	glVertex3fv( v );
	glVertex3fv( v + 6 );
	glVertex3fv( v + 3 );

	glVertex3fv( v + 6 );
	glVertex3fv( v + 9 );
	glVertex3fv( v + 3 );

}
