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

#include "StarsActuator.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "game/Phase.h"
#include "../SpaceGamePhase.h"
#include "console/console.h"

using namespace ceres;

StarsActuator::StarsActuator() {
}

StarsActuator::~StarsActuator(){
}

bool StarsActuator::init( float dt, float time, std::string &error ) {

	object = NULL;

	Phase *phase1 = (Phase *)game;

	fieldSize = 200;

	stars = new Stars();
	stars->material = new GL1Material();
	stars->material->illuminated = false;
	stars->material->diffuse.set( 1, 1, 1 );

	int numStars = 100;
	for ( int32_t i  = 0; i < numStars; i ++ ) {
		Vector3 star;
		star.set( ( 2 * frand() - 1 ), ( 2 * frand() - 1 ), ( 2 * frand() - 1 ) )->multiplyScalar( fieldSize );
		if ( spaceshipActuator ) star.inc( &spaceshipActuator->spaceshipObject->pose->position );
		stars->stars.push_back( star );
	}

	phase1->scene->objects.push_back( stars );

	phase1->actuatorsToBeAdded.push_back( this );


	return true;

}

void StarsActuator::actuate( float dt, float time ) {

	if ( spaceshipActuator ) {

		Vector3 *pos = &spaceshipActuator->spaceshipObject->pose->position;

		float s2 = 2 * fieldSize;
		std::vector<Vector3> &s = stars->stars;
		for ( int32_t i = 0, n = s.size(); i < n; i ++ ) {
			Vector3 *star = &s[ i ];
			Vector3 p;
			p.sub( star, pos );
			if ( abs( p.x ) < fieldSize * 0.2 && abs( p.y ) < fieldSize * 0.2 ) star->z += s2;
			if ( p.x < - fieldSize ) star->x += s2;
			else if ( p.x > fieldSize ) star->x -= s2;
			if ( p.y < - fieldSize ) star->y += s2;
			else if ( p.y > fieldSize ) star->y -= s2;
			if ( p.z < - fieldSize ) star->z += s2;
			else if ( p.z > fieldSize ) star->z -= s2;
		}

	}

}

void StarsActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( stars );

}
