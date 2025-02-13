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

#include "EnemyActuator.h"
#include "../SpaceGamePhase.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "../actuators/BulletActuator.h"

#include "console/console.h"
#include "time/time.h"

using namespace ceres;

EnemyActuator::EnemyActuator() : enemyType( 1 ) {
}

EnemyActuator::~EnemyActuator(){
}

bool EnemyActuator::init( float dt, float time, std::string &error ) {

	GL1Material *enemyMaterial = new GL1Material();
	switch ( enemyType ) {
		case 1:
			enemyMaterial->diffuse.set( 0.7, 0.0, 0.1 );
			maxVelocity = 100.0;
			break;

		case 2:
			enemyMaterial->diffuse.set( 0.4 + 0.6 * frand(), 0.4 + 0.6 * frand(), 0.4 + 0.6 * frand() );
			maxVelocity = 25.0;
			break;
		default:
			break;
	}
	enemyMaterial->minZ = 15;
	enemyMaterial->maxZ = 500;
	//enemyMaterial->specular.set( 0.0, 0.0, 0.0 );

	GL1ObjectUtils objectUtils;
	Vector3 enemyPos;
	enemyObject = objectUtils.createObject(
		SPACESHIP_MADNESS_DIR + std::string( "stls" ) + std::string( "/enemy" ) + std::to_string( enemyType ) + std::string( ".stl" ),
		enemyMaterial,
		enemyType == 1 ? 25.0 : 10,
		&enemyPos,
		error,
		new GL1Mesh(),
		enemyType == 2 ? 3.0 : 1.0
	);
	if ( ! enemyObject ) return false;

	object = enemyObject;

	randomDirection.set( frand() * 2.0 - 1.0, frand() * 2.0 - 1.0, frand() * 2.0 - 1.0 )->normalize();

	Phase *phase1 = (Phase *)game;

	buildings = phase1->collisionables[ "buildings" ];
	starship = phase1->collisionables[ "starship" ];
	laser1 = phase1->collisionables[ "laser1" ];
	laser2 = phase1->collisionables[ "laser2" ];

	if ( phase1->scene ) phase1->scene->objects.push_back( enemyObject );
	phase1->actuatorsToBeAdded.push_back( this );

	((SpaceGamePhase *)game)->enemies.push_back( this );

	health = 1.0;

	enabled = true;

	timeForNextBulletShot = time + 12 + frand() * 5;

	return true;

}

void EnemyActuator::actuate( float dt, float time ) {

	if ( ! enabled ) return;

	Vector3 *p = &enemyObject->pose->position;

	if ( health > 0 ) {

		Vector3 incRandom( frand() * 2.0 - 1.0, frand() * 2.0 - 1.0, frand() * 2.0 - 1.0 );
		incRandom.multiplyScalar( 0.05 );
		randomDirection.inc( &incRandom );
		Vector3 min1( -1, -1, -1 );
		Vector3 max1( 1, 1, 1 );
		randomDirection.clamp( &min1, &max1 )->normalize();

		Vector3 relPos;
		if ( spaceshipActuator && ! spaceshipActuator->hasBeenHit ) relPos.sub( &spaceshipActuator->spaceshipObject->pose->position, p )->normalize();
		relPos.multiplyScalar( 0.85 );
		Vector3 random2 = randomDirection;
		random2.multiplyScalar( 0.15 );
		relPos.inc( &random2 );
		Vector3 accelDt;
		accelDt.copy( &relPos )->multiplyScalar( 250.0 * dt );
		velocity.inc( &accelDt );
		float v = velocity.length();
		if ( v > maxVelocity ) {
			v = maxVelocity;
			velocity.normalize()->multiplyScalar( v );
		}
		relPos.multiplyScalar( v );
		velocity.lerp( &velocity, &relPos, 0.05 );

		if ( time >= timeForNextBulletShot ) {

			if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "enemyShot" ), 0.6 ) ) {
				println( "Could not play enemyShot.wav" );
			}

			BulletActuator *bulletActuator = new BulletActuator();
			bulletActuator->game = this->game;
			bulletActuator->spaceshipActuator = this->spaceshipActuator;
			std::string error;
			bulletActuator->init( dt, time, error );
			bulletActuator->bulletObject->pose->position.copy( p );
			bulletActuator->bulletObject->pose->updateMatrix();
			Vector3 relPos;
			float bulletVelocity = 140;
			relPos.sub( &spaceshipActuator->spaceshipObject->pose->position, &bulletActuator->bulletObject->pose->position );
			bulletActuator->velocity.copy( &relPos )->normalize()->multiplyScalar( bulletVelocity );

			timeForNextBulletShot = time + 12 + frand() * 5;
		}

	} else {
		velocity.y -= 9.8 * dt;
		if ( time > timeToDestroy ) ((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		else SpaceGamePhase::explodeMeshRandomly( enemyObject->mesh, 0.12 * ( enemyType == 2 ? 0.1 : 1 ), dt );
	}

	// Dynamics

	Vector3 increm( velocity );
	increm.multiplyScalar( dt );

	enemyObject->pose->position.inc( &increm );

	// Collisions

	// Collision with ground
	const float bigDelta = 0.1;
	if ( p->y < 0.0 ) {
		p->y = bigDelta;
		velocity.y *= - 0.4;
	}
	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;
	if ( p->y > phase1->ceilingHeight ) {
		p->y = phase1->ceilingHeight - bigDelta;
		velocity.y *= - 0.4;
	}

	// Collision with buildings
	if ( buildings ) {
		if ( buildings->isPointInside( p ) ) {
			if ( time == 0.0 ) {
				enemyObject->pose->position.y += 200.0;
			} else {
				velocity.multiplyScalar( - 1.2 );
				p->dec( &increm );
			}

		}
	}

	// Collision with the ship
	if ( starship ) {
		if ( starship->intersectsSphere( &enemyObject->pose->position, enemyObject->radius ) ) {
			if ( health > 0 ) {
				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ), 0.82 ) ) {
					println( "Could not play bigExplosion" );
				}
			}
			float damage = 0.25 * ( health <= 0 ? 1.0 - ( timeToDestroy - time ) / 2.5 : 1.0 );
			spaceshipActuator->receiveHit( damage, dt, time );
			((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		}
	}

	// Collision with the laser
	if ( laser1 ) {
		if ( laser1->intersectsSphere( &enemyObject->pose->position, enemyObject->radius ) ) {
			receiveHit( dt * 3.0, dt, time );
		}
	}
	if ( laser2 ) {
		if ( laser2->intersectsSphere( &enemyObject->pose->position, enemyObject->radius ) ) {
			receiveHit( dt * 3.0, dt, time );
		}
	}

	enemyObject->pose->updateMatrix();

}

void EnemyActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( enemyObject );

	std::vector<Actuator *> &enemies = ((SpaceGamePhase *)game)->enemies;
	for ( uint32_t i = 0, n = enemies.size(); i < n; i ++ ) {
		if ( ( enemies[ i ] ) == this ) {
			enemies.erase( enemies.begin() + i );
			break;
		}
	}
}

void EnemyActuator::receiveHit( float amount, float dt, float time ) {

	if ( health > 0 ) {

		health -= amount;

		if ( health <= 0 ) {

			health = 0;
			timeToDestroy = time + 2.5;

			name = "x";

			enemyObject->material->diffuse.set( 0.4, 0.2, 0.15 );
			enemyObject->material->doubleSided = false;

			if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "explosion" ) ) ) {
				println( "Could not play explosion" );
			}

			spaceshipActuator->parameters.score += 500;
			addRumble(1.0f);

		}

	}

}
