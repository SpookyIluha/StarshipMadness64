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

#include "HUD.h"

#include <GL/gl.h>

using namespace ceres;

HUD::HUD() {
}

HUD::~HUD(){
}

void HUD::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	Vector3 center;
	center = camera->poseMatrixInverse.vector3XMatrix4( &pose->position );

	Vector3 up( 0, 1, 0 );
	up.multiplyScalar( radius );
	Vector3 up2( 0, 1, 0 );
	up2.multiplyScalar( radius  * 0.7 );
	Vector3 side( 1, 0, 0 );
	side.multiplyScalar( radius );
	Vector3 side2( 1, 0, 0 );
	side2.multiplyScalar( radius * 0.7 );

	Vector3 p0, p1, p2;
	float v[ 9 ];

	glBegin( GL_TRIANGLES );

	p0.copy( &center )->inc( &up )->dec( &side );
	p1.copy( &center )->inc( &up2 )->dec( &side );
	p2.copy( &center )->inc( &up )->dec( &side2 );
	p0.copyToArray( v );
	p1.copyToArray( v + 3 );
	p2.copyToArray( v + 6 );

	glVertex3fv( v );
	glVertex3fv( v + 3 );
	glVertex3fv( v + 6 );

	p0.copy( &center )->inc( &up )->inc( &side );
	p1.copy( &center )->inc( &up2 )->inc( &side );
	p2.copy( &center )->inc( &up )->inc( &side2 );
	p0.copyToArray( v );
	p1.copyToArray( v + 3 );
	p2.copyToArray( v + 6 );

	glVertex3fv( v );
	glVertex3fv( v + 3 );
	glVertex3fv( v + 6 );

	p0.copy( &center )->dec( &up )->dec( &side );
	p1.copy( &center )->dec( &up2 )->dec( &side );
	p2.copy( &center )->dec( &up )->dec( &side2 );
	p0.copyToArray( v );
	p1.copyToArray( v + 3 );
	p2.copyToArray( v + 6 );

	glVertex3fv( v );
	glVertex3fv( v + 3 );
	glVertex3fv( v + 6 );

	p0.copy( &center )->dec( &up )->inc( &side );
	p1.copy( &center )->dec( &up2 )->inc( &side );
	p2.copy( &center )->dec( &up )->inc( &side2 );
	p0.copyToArray( v );
	p1.copyToArray( v + 3 );
	p2.copyToArray( v + 6 );

	glVertex3fv( v );
	glVertex3fv( v + 3 );
	glVertex3fv( v + 6 );

	glEnd();

	finishRender();

}

void HUD::setupRender( Camera *camera ) {

	if ( material ) material->beginUse();

}

void HUD::finishRender() {

	if ( material ) material->endUse();

}
