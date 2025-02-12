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

#include "ProgressBar.h"

#include <GL/gl.h>
#include <GL/gl_integration.h>

using namespace ceres;

ProgressBar::ProgressBar():
	maxSize( 1.0 ),
	currentSize( 1.0 ),
	currentValue( 1.0 ) {
}

ProgressBar::~ProgressBar(){
}


void ProgressBar::render( Camera *camera ) {

	if ( ! visible ) return;

	//setupRender( camera );

	float rgba[ 4 ];
	//GraphicsColor black( 0.2, 0.2, 0.3, 1.0 );
	material->emission.toArray( rgba, true );

	int margpx = 5;
	
	gl_context_end();
	rdpq_set_mode_fill(RGBA32(50, 50, 75, 0));
	rdpq_fill_rectangle(cornerNW.x - margpx, cornerNW.y - margpx, cornerNW.x + margpx + 500 * currentSize, cornerNW.y + margpx + 10);
	rdpq_set_mode_fill(RGBA32((uint8_t)(rgba[0] * 255), (uint8_t)(rgba[1] * 255), (uint8_t)(rgba[2] * 255), 0));
	rdpq_fill_rectangle(cornerNW.x, cornerNW.y, cornerNW.x + 500 * currentValue, cornerNW.y + 10);
	//assertf(0, "hp %.2f, %.2f", currentValue, currentSize );
	//assertf(0, "pos: %.2f, %.2f,%.2f,%.2f", cornerNW.x - margin, cornerNW.y - margin, cornerSE.x + margin, cornerNW.y + margin);
	gl_context_begin();

	/*
	glColor4fv( rgba );
	glBegin( GL_TRIANGLES );

	glNormal3f( 0.0, 0.0, 1.0 );

	glVertex3f( cornerNW.x - margin, cornerNW.y + margin, cornerNW.z );
	glVertex3f( cornerNW.x - margin, cornerSE.y - margin, cornerNW.z );
	glVertex3f( cornerSE.x + margin, cornerNW.y + margin, cornerNW.z );

	glVertex3f( cornerNW.x - margin, cornerSE.y - margin, cornerNW.z );
	glVertex3f( cornerSE.x + margin, cornerSE.y - margin, cornerNW.z );
	glVertex3f( cornerSE.x + margin, cornerNW.y + margin, cornerNW.z );

	glEnd();

	material->emission.toArray( rgba, true );
	glColor4fv( rgba );

	float valueX = cornerNW.x + ( cornerSE.x - cornerNW.x ) * currentValue / currentSize;

	glBegin( GL_TRIANGLES );

	glVertex3f( cornerNW.x, cornerNW.y, cornerNW.z );
	glVertex3f( cornerNW.x, cornerSE.y, cornerNW.z );
	glVertex3f( valueX, cornerNW.y, cornerNW.z );

	glVertex3f( cornerNW.x, cornerSE.y, cornerNW.z );
	glVertex3f( valueX, cornerSE.y, cornerNW.z );
	glVertex3f( valueX, cornerNW.y, cornerNW.z );

	glEnd();*/

	//finishRender();

}

void ProgressBar::setupRender( Camera *camera ) {

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	if ( material ) material->beginUse();

	glDisable( GL_DEPTH_TEST );

}

void ProgressBar::finishRender() {

	glEnable( GL_DEPTH_TEST );

	if ( material ) material->endUse();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

}
