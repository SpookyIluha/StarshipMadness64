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

#include "BannerMesh.h"

#include <GL/gl.h>

using namespace ceres;

BannerMesh::BannerMesh() {
}

BannerMesh::~BannerMesh(){
}

void BannerMesh::setupRender( Camera *camera ) {

	// This object is in eye space. No use for the camera matrix

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();

	glDisable( GL_DEPTH_TEST );

}

void BannerMesh::finishRender() {

	glEnable( GL_DEPTH_TEST );

	if ( material ) material->endUse();

	glPopMatrix();

}
