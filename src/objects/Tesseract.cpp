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

#include "Tesseract.h"

#include <GL/gl.h>
#include "console/console.h"

using namespace ceres;

Link::Link( int32_t point0, int32_t point1 ) {
	this->point0 = point0;
	this->point1 = point1;
}

Tesseract::Tesseract() :
	thickness( 10.0 ) {

	points.push_back( Vector3( 0, 0, 0 ) );
	points.push_back( Vector3( 0, 0, 1 ) );
	points.push_back( Vector3( 0, 1, 0 ) );
	points.push_back( Vector3( 0, 1, 1 ) );
	points.push_back( Vector3( 1, 0, 0 ) );
	points.push_back( Vector3( 1, 0, 1 ) );
	points.push_back( Vector3( 1, 1, 0 ) );
	points.push_back( Vector3( 1, 1, 1 ) );

	points.push_back( Vector3( 0, 0, 0 ) );
	points.push_back( Vector3( 0, 0, 1 ) );
	points.push_back( Vector3( 0, 1, 0 ) );
	points.push_back( Vector3( 0, 1, 1 ) );
	points.push_back( Vector3( 1, 0, 0 ) );
	points.push_back( Vector3( 1, 0, 1 ) );
	points.push_back( Vector3( 1, 1, 0 ) );
	points.push_back( Vector3( 1, 1, 1 ) );

	for ( int32_t i = 0, n  = points.size(); i < n; i ++ ) rotatedPoints.push_back( Vector3() );
	for ( int32_t i = 0, n  = points.size(); i < n; i ++ ) transformedPoints.push_back( Vector3() );

	links.push_back( Link( 0, 1 ) );
	links.push_back( Link( 2, 3 ) );
	links.push_back( Link( 4, 5 ) );
	links.push_back( Link( 6, 7 ) );

	links.push_back( Link( 0, 2 ) );
	links.push_back( Link( 4, 6 ) );
	links.push_back( Link( 1, 3 ) );
	links.push_back( Link( 5, 7 ) );

	links.push_back( Link( 0, 4 ) );
	links.push_back( Link( 1, 5 ) );
	links.push_back( Link( 2, 6 ) );
	links.push_back( Link( 3, 7 ) );


	links.push_back( Link( 0, 8 ) );
	links.push_back( Link( 1, 9 ) );
	links.push_back( Link( 2, 10 ) );
	links.push_back( Link( 3, 11 ) );
	links.push_back( Link( 4, 12 ) );
	links.push_back( Link( 5, 13 ) );
	links.push_back( Link( 6, 14 ) );
	links.push_back( Link( 7, 15 ) );


	links.push_back( Link( 8, 9 ) );
	links.push_back( Link( 10, 11 ) );
	links.push_back( Link( 12, 13 ) );
	links.push_back( Link( 14, 15 ) );

	links.push_back( Link( 8, 10 ) );
	links.push_back( Link( 12, 14 ) );
	links.push_back( Link( 9, 11 ) );
	links.push_back( Link( 13, 15 ) );

	links.push_back( Link( 8, 12 ) );
	links.push_back( Link( 9, 13 ) );
	links.push_back( Link( 10, 14 ) );
	links.push_back( Link( 11, 15 ) );

}

Tesseract::~Tesseract(){
	if(block) rspq_block_free(block);
	block = NULL;
}

void Tesseract::transformPoints() {

	//Matrix4 m1;
	//m1.addTraslation( - size * 0.5, - size * 0.5, - size * 0.5 );
	//Matrix4 m2;
	//m2.multiply( &m1, &pose0.matrix );
	for ( int32_t i = 0, n = 8; i < n; i ++ ) rotatedPoints[ i ] = pose0.matrix.vector3XMatrix4( &points[ i ] );
	//m2.multiply( &m1, &pose1.matrix );
	for ( int32_t i = 8, n = 16; i < n; i ++ ) rotatedPoints[ i ] = pose1.matrix.vector3XMatrix4( &points[ i ] );

	for ( int32_t i = 0, n = 8; i < n; i ++ ) transformedPoints[ i ] = pose->matrix.vector3XMatrix4( &rotatedPoints[ i ] );
	for ( int32_t i = 8, n = 16; i < n; i ++ ) transformedPoints[ i ] = pose->matrix.vector3XMatrix4( &rotatedPoints[ i ] );

}

void Tesseract::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );
	//if(!block){
	//	rspq_block_begin();
		for ( int32_t j = 0, n = links.size(); j < n; j ++ ) {

			Link *link = &links[ j ];
	
			Vector3 p0 = &rotatedPoints[ link->point0 ];
			Vector3 p1 = &rotatedPoints[ link->point1 ];
	
			int32_t numSides = 12;
			float dAngle = 2.0 * M_PI / numSides;
			float angle = 0;
			Vector3 v0, v1, v2, v3;
			for ( int32_t i =  0; i < numSides + 1; i ++ ) {
	
				Vector3 dPos;
				dPos.sub( &p1, &p0 )->normalize();
				Vector3 normal;
				normal.set( 1, 1, 1 );
				Vector3 axisZ;
				axisZ.crossProduct( &dPos, &normal );
				if ( axisZ.length()  < 0.00001 ) {
					normal.set( 1, 0, 0 );
					axisZ.crossProduct( &dPos, &normal );
				}
				normal.crossProduct( &axisZ, &dPos );
	
				Vector3 vertPos;
				vertPos.copy( &normal )->multiplyScalar( thickness * sin( angle ) );
				Vector3 yAxis;
				yAxis.copy( &axisZ )->multiplyScalar( thickness * cos( angle ) );
				vertPos.inc( &yAxis );
				Vector3 vertPos2;
				Vector3 normalRadial;
				normalRadial.copy( &vertPos )->normalize();
				vertPos2.copy( &vertPos )->inc( &p0 );
				vertPos.inc( &p1 );
	
				angle += dAngle;
	
				if ( i == 0 ) {
					v0.copy( &vertPos );
					v1.copy( &vertPos2 );
					continue;
				} else {
					v2.copy( &vertPos );
					v3.copy( &vertPos2 );
				}
	
				float v[ 12 ];
				float n[ 3 ];
	
				glBegin( GL_TRIANGLES );
	
				v[ 0 ] = v0.x;
				v[ 1 ] = v0.y;
				v[ 2 ] = v0.z;
				v[ 3 ] = v1.x;
				v[ 4 ] = v1.y;
				v[ 5 ] = v1.z;
				v[ 6 ] = v2.x;
				v[ 7 ] = v2.y;
				v[ 8 ] = v2.z;
				v[ 9 ] = v3.x;
				v[ 10 ] = v3.y;
				v[ 11 ] = v3.z;
	
				n[ 0 ] = normalRadial.x;
				n[ 1 ] = normalRadial.y;
				n[ 2 ] = normalRadial.z;
	
				drawQuad( v, n );
	
				glEnd();
	
				v0.copy( &v2 );
				v1.copy( &v3 );
	
			}
	
		}
	//	block = rspq_block_end();
	//} rspq_block_run(block);
	

	finishRender();

}

void Tesseract::setupRender( Camera *camera ) {

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

}

void Tesseract::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}

void Tesseract::drawQuad( float *v, float *normal ) {

	glNormal3fv( normal );

	glVertex3fv( v );
	glVertex3fv( v + 6 );
	glVertex3fv( v + 3 );

	glVertex3fv( v + 6 );
	glVertex3fv( v + 9 );
	glVertex3fv( v + 3 );

}
