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

#include "Scene.h"

using namespace ceres;

Scene::Scene() {
}

Scene::~Scene() {

	for ( int32_t i = 0, il = getNumObjects(); i < il; i ++ ) {

		delete objects[ i ];

	}

}

void Scene::render( Camera *camera ) {

	for ( int32_t i = 0, il = getNumObjects(); i < il; i ++ ) {

		objects[ i ]->render( camera );

	}

}

bool Scene::deleteObject( Object3D *object ) {

	// Returns true iff object found and deleted

	if ( ! object ) return false;

	int32_t index = findObjectIndex( object );
	if ( index < 0 ) return false;

	delete object;
	objects.erase( objects.begin() + index );

	return true;

}

int32_t Scene::findObjectIndex( Object3D *object ) {

	for ( uint32_t i = 0; i < objects.size(); i ++ ) {

		if ( objects[ i ] == object ) return i;

	}

	return -1;

}
