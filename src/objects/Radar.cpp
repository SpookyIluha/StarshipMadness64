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

#include "Radar.h"

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>
//#include <GL/glkos.h>

using namespace ceres;

Radar::Radar() :
	size( 5 ),
	margin( 0.01 ),
	range( 1500 ) {
}

Radar::~Radar(){
}


void Radar::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	float halfSize = size * 0.5;

	glBegin( GL_TRIANGLES );

	float rgba[ 4 ];
	GraphicsColor black( 0.2, 0.2, 0.3, 1.0 );
	black.toArray( rgba, true );
	glColor4fv( rgba );

	glNormal3f( 0.0, 1.0, 0.0 );

	glVertex3f( - halfSize - margin, 0, - halfSize - margin );
	glVertex3f( - halfSize - margin, 0, + halfSize + margin );
	glVertex3f( halfSize + margin, 0, halfSize + margin );

	glVertex3f( - halfSize - margin, 0, - halfSize - margin );
	glVertex3f( + halfSize + margin, 0, + halfSize + margin );
	glVertex3f( halfSize + margin, 0, - halfSize - margin );

	glEnd();

	glBegin( GL_POINTS );
	for ( int32_t i = 0, n = points.size(); i < n; i ++ ) {

		RadarPoint *point = &points[ i ];

		rgba[ 0 ] = point->color.x;
		rgba[ 1 ] = point->color.y;
		rgba[ 2 ] = point->color.z;
		rgba[ 3 ] = 1;
		glColor4fv( rgba );

		glVertex3f( point->position.x * halfSize, point->position.y * halfSize, point->position.z * halfSize );

	}

	rgba[ 0 ] = 1;
	rgba[ 1 ] = 1;
	rgba[ 2 ] = 1;
	rgba[ 3 ] = 1;
	glColor4fv( rgba );
	glVertex3f( 0, 0, 0 );
	glEnd();

	material->emission.toArray( rgba, true );
	glColor4fv( rgba );
	glBegin( GL_LINES );
	for ( int32_t i = 0, n = points.size(); i < n; i ++ ) {

		RadarPoint *point = &points[ i ];

		rgba[ 0 ] = point->color.x;
		rgba[ 1 ] = point->color.y;
		rgba[ 2 ] = point->color.z;
		rgba[ 3 ] = 1;
		glColor4fv( rgba );

		glVertex3f( point->position.x * halfSize, point->position.y * halfSize, point->position.z * halfSize );
		glVertex3f( point->position.x * halfSize, 0, point->position.z * halfSize );

	}
	glEnd();

	finishRender();

}

void Radar::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	if ( pose ) glLoadMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

	glDisable( GL_DEPTH_TEST );

	glPointSize( 5.0 );
	glLineWidth( 3.0 );

}

void Radar::finishRender() {

	if ( material ) material->endUse();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );

}
