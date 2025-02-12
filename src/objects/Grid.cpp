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

#include "Grid.h"

#include <GL/gl.h>

using namespace ceres;

Grid::Grid() :
	height( 0 ),
	color( 0, 1, 0 ),
	squareLength( 40.0 ),
	squareLineWidth( 2.0 ),
	zViewDistance( 500 ) {
}

Grid::~Grid(){
}


void Grid::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );
	drawGrid( camera, height, &color );
	finishRender();

}

void Grid::setupRender( Camera *camera ) {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

	glDisable(GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );

}

void Grid::finishRender() {

	glEnable( GL_CULL_FACE );
	glEnable(GL_DEPTH_TEST);
	if ( material ) material->endUse();

	glPopMatrix();

}

void Grid::drawGrid( Camera *camera, float yCoordinate, GraphicsColor *color ) {

	Vector3 offset;
	if ( pose ) {
		Vector3 traslation = pose->matrix.getTraslation();
		double i1, i2;
		yCoordinate -= traslation.y;
		offset.set( modf( - traslation.x / squareLength, &i1 ), 0.0, modf( - traslation.z / squareLength, &i2 ) )->multiplyScalar( squareLength );
	}
	float halfSquareLineWidth = 0.5 * squareLineWidth;
	glDisable( GL_LIGHTING );
	glBegin( GL_TRIANGLES );
	glColor4f( color->r, color->g, color->b, 1.0 );
	for ( float z = - zViewDistance + offset.z, z1 = zViewDistance + offset.z; z < z1; z += squareLength ) {
		Vector3 v0( - zViewDistance, yCoordinate, z );
		Vector3 v1( zViewDistance, yCoordinate, z );
		Vector3 halfSide( 0, 0, - halfSquareLineWidth );
		drawHorizontalLine( &v0, &v1, &halfSide );
	}
	for ( float x = - zViewDistance + offset.x, x1 = zViewDistance + offset.x; x < x1; x += squareLength ) {
		Vector3 v0( x, yCoordinate, - zViewDistance );
		Vector3 v1( x, yCoordinate, zViewDistance );
		Vector3 halfSide( halfSquareLineWidth, 0, 0 );
		drawHorizontalLine( &v0, &v1, &halfSide );
	}
	glEnd();
	glEnable( GL_LIGHTING );

}

void Grid::drawHorizontalLine( Vector3  *point1, Vector3 *point2, Vector3 *halfSide ) {

	Vector3 points[ 4 ];
	points[ 0 ].copy( point1 )->dec( halfSide );
	points[ 1 ].copy( point1 )->inc( halfSide );
	points[ 2 ].copy( point2 )->dec( halfSide );
	points[ 3 ].copy( point2 )->inc( halfSide );

	glVertex3f( points[ 0 ].x, points[ 0 ].y, points[ 0 ].z );
	glVertex3f( points[ 2 ].x, points[ 2 ].y, points[ 2 ].z );
	glVertex3f( points[ 1 ].x, points[ 1 ].y, points[ 1 ].z );

	glVertex3f( points[ 2 ].x, points[ 2 ].y, points[ 2 ].z );
	glVertex3f( points[ 3 ].x, points[ 3 ].y, points[ 3 ].z );
	glVertex3f( points[ 1 ].x, points[ 1 ].y, points[ 1 ].z );

}
