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

#include "Bullet.h"

#include <GL/gl.h>

using namespace ceres;

Bullet::Bullet() {
}

Bullet::~Bullet(){
}

void Bullet::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	Vector3 center;
	center = camera->poseMatrixInverse.vector3XMatrix4( &pose->position );

	Vector3 up( 0, 1, 0 );
	up.multiplyScalar( radius );
	Vector3 side( 1, 0, 0 );
	side.multiplyScalar( radius );

	Vector3 p0, p1, p2, p3;

	p0.copy( &center )->inc( &up )->dec( &side );
	p1.copy( &center )->inc( &up )->inc( &side );
	p2.copy( &center )->dec( &up )->dec( &side );
	p3.copy( &center )->dec( &up )->inc( &side );

	float v[ 12 ];
	float uv[ 8 ];

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

	uv[ 0 ] = 0;
	uv[ 1 ] = 0;
	uv[ 2 ] = 1;
	uv[ 3 ] = 0;
	uv[ 4 ] = 0;
	uv[ 5 ] = 1;
	uv[ 6 ] = 1;
	uv[ 7 ] = 1;

	drawQuad( v, uv );

	glEnd();

	finishRender();

}

void Bullet::setupRender( Camera *camera ) {

	if ( material ) material->beginUse();

}

void Bullet::finishRender() {

	if ( material ) material->endUse();

}

void Bullet::drawQuad( float *v, float *uv ) {

	glTexCoord2fv( uv );
	glVertex3fv( v );
	glTexCoord2fv( uv + 4 );
	glVertex3fv( v + 6 );
	glTexCoord2fv( uv + 2 );
	glVertex3fv( v + 3 );

	glTexCoord2fv( uv + 4 );
	glVertex3fv( v + 6 );
	glTexCoord2fv( uv + 6 );
	glVertex3fv( v + 9 );
	glTexCoord2fv( uv + 2 );
	glVertex3fv( v + 3 );

}
