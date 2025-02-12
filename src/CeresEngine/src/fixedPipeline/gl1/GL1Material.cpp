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

#include "GL1Material.h"
#include "GL1Texture.h"

#include <GL/gl.h>
#include <GL/glu.h>

using namespace ceres;

void GL1Material::beginUse() {

	float rgba[ 4 ];

	if ( transparent ) {
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if ( ! illuminated ) {
		glDisable( GL_LIGHTING );
		diffuse.toArray( rgba, true );
		glColor4fv( rgba );
	} else {

		ambient.toArray( rgba, true );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, rgba );

		diffuse.toArray( rgba, true );
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, rgba );

		//specular.toArray( rgba, true );
		//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, rgba );

		emission.toArray( rgba, true );
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, rgba );

		//glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, specularExponent );
	}

	if ( doubleSided ) glDisable( GL_CULL_FACE );

	glEnable(GL_TEXTURE_2D);
	if ( texture ) glBindTexture( GL_TEXTURE_2D, ((GL1Texture *)texture)->textureName );
	else glDisable(GL_TEXTURE_2D);
	//else glBindTexture( GL_TEXTURE_2D, 0 );

	glMatrixMode(GL_TEXTURE);
	glTranslatef(transX, transY, 0);
	//glTexSizeN64(surf.width, surf.height);
	glMatrixMode(GL_MODELVIEW);

	if(minZ > 0 && maxZ > 0){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70, 1.33333f, minZ, maxZ);
		glMatrixMode(GL_MODELVIEW);
	}

	if ( ! depthTest ) {
		glDisable( GL_DEPTH_TEST );
		glDepthFunc(GL_ALWAYS);
	}
}

void GL1Material::endUse() {

	if ( ! depthTest ) {
		glEnable( GL_DEPTH_TEST );
		glDepthFunc(GL_LESS);
	}

	if ( transparent ) glDisable( GL_BLEND );

	if ( ! illuminated ) glEnable( GL_LIGHTING );

	if ( doubleSided ) glEnable( GL_CULL_FACE );

	if ( texture ) glBindTexture( GL_TEXTURE_2D, 0 );

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	if(minZ > 0 && maxZ > 0){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70, 1.33333f, 7, 500);
		glMatrixMode(GL_MODELVIEW);
	}

}
