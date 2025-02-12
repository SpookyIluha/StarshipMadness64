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

#include "GL1Text3D.h"

#include "console/console.h"

#include <GL/gl.h>
#include <GL/gl_integration.h>

using namespace ceres;

GL1Text3D::GL1Text3D() {
}

GL1Text3D::~GL1Text3D() {
	delete [] charsIndexes;
}

void GL1Text3D::init( FPFont *font, FPMaterial *material, Vector3 scale) {

	// Material must be textured

	this->font = font;
	this->material = material;

	maxChars = 0;
	currentChars = 0;
	charsIndexes = NULL;

	this->scale = scale;

}

void GL1Text3D::setString( std::string s ) {

	currentContentString = s;

	currentChars = s.length();

	if ( charsIndexes == NULL || currentChars > maxChars ) {
		bool doubleCount = charsIndexes != NULL;
		delete [] charsIndexes;
		maxChars = currentChars * ( doubleCount ? 2 : 1 );
		charsIndexes = new int32_t[ maxChars ];
	}

	for ( int32_t i = 0; i < currentChars; i ++ ) {
		std::string s = currentContentString.substr( i, 1 );
		if ( ((char)s[ 0 ]) == 10 ) charsIndexes[ i ] = font->newlineIndex;
		else charsIndexes[ i ] = font->characterIndexes[ s ];
	}

}

void GL1Text3D::setupRender( Camera *camera ) {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();
}

void GL1Text3D::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}

void GL1Text3D::render( Camera *camera ) {

	if ( ! visible || currentChars <= 0 ) return;

	gl_context_end();
	Vector3 textpos;
	textpos.x = pose->position.x; //* 500 / (-pose->position.z);
	textpos.y = pose->position.y; //* 500 / (pose->position.z);

	rdpq_fontstyle_t style;
	style.outline_color = RGBA32(0,0,0,255);
	style.color = RGBA32((uint8_t)(this->material->diffuse.r * 255), (uint8_t)(this->material->diffuse.g * 255), (uint8_t)(this->material->diffuse.b * 255), 255);
	rdpq_font_style(pose->scale? font->RDPQfontLarge : font->RDPQfontSmall, 0, &style);
	rdpq_text_print(NULL, pose->scale? 2 : 3, textpos.x + 0, textpos.y + 0, currentContentString.c_str());

	gl_context_begin();
/*
	setupRender( camera );

	int32_t newlineIndex = font->newlineIndex;

	glBegin( GL_TRIANGLES );
	glNormal3f( 0, 0, 1 );

	//float uPixel = 1.0 / ((float)font->texture->resolutionX);
	//float vPixel = 1.0 / ((float)font->texture->resolutionY);
	float lineHeight = scale.y;// * vPixel;

	Vector3 pos[ 4 ];
	pos[ 0 ].y = lineHeight;
	pos[ 1 ].y = lineHeight;
	for ( int32_t i = 0; i < currentChars; i ++ ) {

		int32_t charIndex = charsIndexes[ i ];
		if ( charIndex == newlineIndex ) {
			pos[ 0 ].x = 0;
			pos[ 1 ].x = 0;
			pos[ 2 ].x = 0;
			pos[ 3 ].x = 0;
			pos[ 0 ].y -= lineHeight;
			pos[ 1 ].y -= lineHeight;
			pos[ 2 ].y -= lineHeight;
			pos[ 3 ].y -= lineHeight;
			continue;
		}

		float dx = lineHeight * font->characterSizes[ charIndex ].x / font->characterSizes[ charIndex ].y;
		pos[ 1 ].x += dx;
		pos[ 3 ].x += dx;

		Vector2 uv0 = font->characterUV0[ charIndex ];
		Vector2 uv1 = font->characterUV1[ charIndex ];

		// 0 2 1

		glTexCoord2f( uv0.x, uv0.y );
		glVertex3f( pos[ 0 ].x, pos[ 0 ].y, pos[ 0 ].z );

		glTexCoord2f( uv0.x, uv1.y );
		glVertex3f( pos[ 2 ].x, pos[ 2 ].y, pos[ 2 ].z );

		glTexCoord2f( uv1.x, uv0.y );
		glVertex3f( pos[ 1 ].x, pos[ 1 ].y, pos[ 1 ].z );

		// 2 3 1

		glTexCoord2f( uv0.x, uv1.y );
		glVertex3f( pos[ 2 ].x, pos[ 2 ].y, pos[ 2 ].z );

		glTexCoord2f( uv1.x, uv1.y );
		glVertex3f( pos[ 3 ].x, pos[ 3 ].y, pos[ 3 ].z );

		glTexCoord2f( uv1.x, uv0.y );
		glVertex3f( pos[ 1 ].x, pos[ 1 ].y, pos[ 1 ].z );

		pos[ 0 ].x = pos[ 1 ].x;
		pos[ 2 ].x = pos[ 3 ].x;

	}

	glEnd();

	finishRender();
*/
}
