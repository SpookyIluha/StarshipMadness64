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

#include "GL1Parallelepiped.h"

#include <GL/gl.h>

#include "console/console.h"

using namespace ceres;

GL1Parallelepiped::GL1Parallelepiped() {
}

GL1Parallelepiped::~GL1Parallelepiped() {
}

void GL1Parallelepiped::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	Vector3 minP = box.getCorner0();
	Vector3 maxP = box.getCorner1();

	float v[ 12 ];
	float normal[ 3 ];

	glBegin( GL_TRIANGLES );

	// N
	v[ 0 ] = maxP.x;
	v[ 1 ] = maxP.y;
	v[ 2 ] = minP.z;
	v[ 3 ] = minP.x;
	v[ 4 ] = maxP.y;
	v[ 5 ] = minP.z;
	v[ 6 ] = maxP.x;
	v[ 7 ] = minP.y;
	v[ 8 ] = minP.z;
	v[ 9 ] = minP.x;
	v[ 10 ] = minP.y;
	v[ 11 ] = minP.z;
	normal[ 0 ] = 0;
	normal[ 1 ] = 0;
	normal[ 2 ] = - 1;
	drawQuad( v, normal );

	// S
	v[ 0 ] = minP.x;
	v[ 1 ] = maxP.y;
	v[ 2 ] = maxP.z;
	v[ 3 ] = maxP.x;
	v[ 4 ] = maxP.y;
	v[ 5 ] = maxP.z;
	v[ 6 ] = minP.x;
	v[ 7 ] = minP.y;
	v[ 8 ] = maxP.z;
	v[ 9 ] = maxP.x;
	v[ 10 ] = minP.y;
	v[ 11 ] = maxP.z;
	normal[ 0 ] = 0;
	normal[ 1 ] = 0;
	normal[ 2 ] = 1;
	drawQuad( v, normal );;

	// W
	v[ 0 ] = minP.x;
	v[ 1 ] = maxP.y;
	v[ 2 ] = minP.z;
	v[ 3 ] = minP.x;
	v[ 4 ] = maxP.y;
	v[ 5 ] = maxP.z;
	v[ 6 ] = minP.x;
	v[ 7 ] = minP.y;
	v[ 8 ] = minP.z;
	v[ 9 ] = minP.x;
	v[ 10 ] = minP.y;
	v[ 11 ] = maxP.z;
	normal[ 0 ] = - 1;
	normal[ 1 ] = 0;
	normal[ 2 ] = 0;
	drawQuad( v, normal );

	// E
	v[ 0 ] = maxP.x;
	v[ 1 ] = maxP.y;
	v[ 2 ] = maxP.z;
	v[ 3 ] = maxP.x;
	v[ 4 ] = maxP.y;
	v[ 5 ] = minP.z;
	v[ 6 ] = maxP.x;
	v[ 7 ] = minP.y;
	v[ 8 ] = maxP.z;
	v[ 9 ] = maxP.x;
	v[ 10 ] = minP.y;
	v[ 11 ] = minP.z;
	normal[ 0 ] = 1;
	normal[ 1 ] = 0;
	normal[ 2 ] = 0;
	drawQuad( v, normal );

	// T
	v[ 0 ] = minP.x;
	v[ 1 ] = maxP.y;
	v[ 2 ] = minP.z;
	v[ 3 ] = maxP.x;
	v[ 4 ] = maxP.y;
	v[ 5 ] = minP.z;
	v[ 6 ] = minP.x;
	v[ 7 ] = maxP.y;
	v[ 8 ] = maxP.z;
	v[ 9 ] = maxP.x;
	v[ 10 ] = maxP.y;
	v[ 11 ] = maxP.z;
	normal[ 0 ] = 0;
	normal[ 1 ] = 1;
	normal[ 2 ] = 0;
	drawQuad( v, normal );

	glEnd();

	finishRender();

}

void GL1Parallelepiped::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();
}

void GL1Parallelepiped::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}

void GL1Parallelepiped::drawQuad( float *v, float *normal ) {

	glNormal3fv( normal );

	glVertex3fv( v );
	glVertex3fv( v + 6 );
	glVertex3fv( v + 3 );

	glVertex3fv( v + 6 );
	glVertex3fv( v + 9 );
	glVertex3fv( v + 3 );

}
