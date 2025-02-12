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

#include "Laser.h"

#include <GL/gl.h>

using namespace ceres;

Laser::Laser():
	ray( Vector3( 50, 0, 0 ) ),
	rayvis( Vector3(50,0,0)),
	thickness( 1.0 ) {
}

Laser::~Laser(){
}

void Laser::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	Matrix4 *m = &camera->poseMatrixInverse;

	Vector3 p0, p1, p2, p3;
	Vector3 rayUnit = m->vector3FreeXMatrix4( &rayvis );
	rayUnit.normalize();

	//Vector3 middlePoint = ray;
	//middlePoint.multiplyScalar( 0.5 )->inc( &beginPoint );
	Vector3 middlePoint = beginPoint;

	Vector3 normal;
	normal = m->vector3XMatrix4( &middlePoint );
	normal.normalize();
	Vector3 side;
	side.crossProduct( &rayUnit, &normal )->multiplyScalar( thickness );
	Vector3 sideWorld = m->vector3FreeXMatrix4Inverse( &side );
	Vector3 endPoint;
	endPoint.add( &beginPoint, &rayvis );
	p0.copy( &beginPoint )->inc( &sideWorld );
	p1.copy( &endPoint )->inc( &sideWorld );
	p2.copy( &beginPoint )->dec( &sideWorld );
	p3.copy( &endPoint )->dec( &sideWorld );

	float v[ 12 ];
	float n[ 3 ];

	glBegin( GL_TRIANGLES );

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

	n[ 0 ] = normal.x;
	n[ 1 ] = normal.y;
	n[ 2 ] = normal.z;
	drawQuad( v, n );

	glEnd();

	finishRender();

}

void Laser::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

}

void Laser::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}

void Laser::drawQuad( float *v, float *normal ) {

	glNormal3fv( normal );

	glVertex3fv( v );
	glVertex3fv( v + 6 );
	glVertex3fv( v + 3 );

	glVertex3fv( v + 6 );
	glVertex3fv( v + 9 );
	glVertex3fv( v + 3 );

}
