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

#include "GL1Scene.h"
#include "GL1Light.h"
#include "GL1Mesh.h"

#include <GL/gl.h>

#include "console/console.h"

using namespace ceres;

GL1Scene::GL1Scene() {
}

GL1Scene::~GL1Scene() {
}

bool GL1Scene::init() {

	// Init lights

	GLint numLights = 0;
	glGetIntegerv( GL_MAX_LIGHTS, &numLights );

	lights.clear();
	for ( int32_t i = 0; i < numLights; i ++ ) {
		lights.push_back( new GL1Light() );
		lights[ i ]->lightIndex = i;

		lights[ i ]->enabled = false;
		lights[ i ]->updateParams();
	}

	return true;

}
