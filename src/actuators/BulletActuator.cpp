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

#include "BulletActuator.h"
#include "../SpaceGamePhase.h"
#include "../objects/Bullet.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Texture.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "console/console.h"

using namespace ceres;

BulletActuator::BulletActuator() {
}

BulletActuator::~BulletActuator(){
}

bool BulletActuator::init( float dt, float time, std::string &error ) {

	GL1Material *bulletMaterial = new GL1Material();
	bulletMaterial->illuminated = false;
	bulletMaterial->doubleSided = true;
	bulletMaterial->diffuse.set( 1, 1, 1 );
	bulletMaterial->specular.set( 0, 0, 0 );
	bulletMaterial->texture = new GL1Texture();
	bulletMaterial->transparent = true;
	bulletMaterial->opacity = 0.5;
	GL1Texture *bulletTexture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "textures/bullet1.png" ), error );
	if ( bulletTexture == NULL ) return NULL;
	bulletMaterial->texture = bulletTexture;

	bulletObject = new Bullet();
	bulletObject->material = bulletMaterial;
	bulletObject->pose = new Pose();
	bulletObject->radius = 15;

	object = bulletObject;

	Phase *phase1 = (Phase *)game;

	buildings = phase1->collisionables[ "buildings" ];
	starship = phase1->collisionables[ "starship" ];

	if ( phase1->scene ) phase1->scene->objects.push_back( bulletObject );
	phase1->actuatorsToBeAdded.push_back( this );


	shotTime = time;

	return true;

}

void BulletActuator::actuate( float dt, float time ) {

	Vector3 *p = &bulletObject->pose->position;

	// Dynamics

	Vector3 increm( velocity );
	increm.multiplyScalar( dt );

	p->inc( &increm );

	bulletObject->pose->updateMatrix();

	// Collisions

	// Collision with ground
	if ( p->y < 0.0 ) {
		if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "explosion" ) ) ) {
			println( "Could not play explosion" );
		}

		((Phase *)game)->actuatorsToBeDeleted.push_back( this );
	}
	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;
	if ( p->y > phase1->ceilingHeight ) {
		if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "explosion" ) ) ) {
			println( "Could not play explosion" );
		}

		((Phase *)game)->actuatorsToBeDeleted.push_back( this );
	}

	// Collision with buildings
	if ( buildings ) {
		if ( buildings->isPointInside( p ) ) {
			if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "explosion" ) ) ) {
				println( "Could not play explosion" );
			}

			((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		}
	}

	// Collision with the ship
	if ( starship ) {
		if ( starship->intersectsSphere( p, bulletObject->radius ) ) {
			if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "explosion" ) ) ) {
				println( "Could not play explosion" );
			}

			spaceshipActuator->receiveHit( 0.25, dt, time );

			((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		}
	}

	// Lifetime
	if ( time > shotTime + 12 ) ((Phase *)game)->actuatorsToBeDeleted.push_back( this );

}

void BulletActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( bulletObject );

	std::vector<Actuator *> &enemies = ((SpaceGamePhase *)game)->enemies;
	for ( uint32_t i = 0, n = enemies.size(); i < n; i ++ ) {
		if ( ( enemies[ i ] ) == this ) {
			enemies.erase( enemies.begin() + i );
			break;
		}
	}
}
