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

#include "Stars.h"

#include <GL/gl.h>
//#include <GL/glkos.h>

using namespace ceres;

Stars::Stars() {
}

Stars::~Stars(){
}

void Stars::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	glBegin( GL_POINTS );

	glNormal3f( 0, 0, 1 );

	for ( int32_t i = 0, n = stars.size(); i < n; i ++ ) {
		Vector3 *star = &stars[ i ];
		glVertex3f( star->x, star->y, star->z );
	}

	glEnd();

	finishRender();

}

void Stars::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

	glPointSize( 2.0 );

}

void Stars::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}
