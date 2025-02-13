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

#include "Enemy3Actuator.h"
#include "../SpaceGamePhase.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"

#include "console/console.h"

using namespace ceres;

Enemy3Actuator::Enemy3Actuator() {
}

Enemy3Actuator::~Enemy3Actuator(){
}

bool Enemy3Actuator::init( float dt, float time, std::string &error ) {

	isDestroying = false;
	timeToDestroy = 0;

	name = "enemy3";

	GL1Material *rodMaterial = new GL1Material();
	rodMaterial->diffuse.set( 0.8, 0.5, 0.0 );
	rodMaterial->minZ = 3;
	rodMaterial->maxZ = 800;
	rodMaterial->depthTest = true;
	//rodMaterial->specular.set( 0.0, 0.0, 0.0 );

	GL1Material *bodyMaterial = new GL1Material();
	bodyMaterial->diffuse.set( 0.1, 0.3, 0.6 );
	bodyMaterial->minZ = 25;
	bodyMaterial->maxZ = 800;
	//bodyMaterial->specular.set( 0.0, 0.0, 0.0 );

	GL1ObjectUtils objectUtils;
	rodObject = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/rod.stl" ), rodMaterial, 500.0, initialPosition, error, new GL1Mesh(), 100.0 );
	if ( ! rodObject ) return false;

	bodyObject = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/body.stl" ), bodyMaterial, 100.0, initialPosition, error, new GL1Mesh(), 100.0 );
	if ( ! bodyObject ) return false;

	object = bodyObject;

	AABBox *box = new AABBox();
	box->corner0.set( -11.75, -250, -11.75 )->inc( &initialPosition );
	box->corner1.set( 11.75, 250, 11.75 )->inc( &initialPosition );
	rodCollisionable = new BoxCollisionable();
	rodCollisionable->boxes.push_back( box );

	bodyCollisionable = new RadiusCollisionable();
	bodyCollisionable->position = &bodyObject->pose->position;
	bodyCollisionable->radius = bodyObject->radius;

	// Balls
	for ( int32_t i = 0; i < 4; i ++ ) {

		GL1Material *ballMaterial = new GL1Material();
		ballMaterial->diffuse.set( 0.0, 1.0, 1.0 );
		ballMaterial->minZ = 15;
		ballMaterial->maxZ = 800;
		//ballMaterial->specular.set( 0.0, 0.0, 0.0 );

		Enemy3Ball *ball = &balls[ i ];
		ball->health = 1.0;
		Vector3 pos;
		pos.set( i & 1 ? - 1 : 1, 0.0, i & 2 ? - 1 : 1 )->multiplyScalar( 90.0 )->inc( &initialPosition );
		ball->object = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/ball.stl" ), ballMaterial, 10.0, pos, error, new GL1Mesh(), 100.0 );

		RadiusCollisionable *collisionable = new RadiusCollisionable();
		collisionable->position = &ball->object->pose->position + 20;
		collisionable->radius = 10.0;
		ball->collisionable = collisionable;

		((Phase *)game)->scene->objects.push_back( ball->object );
	}

	targetY = 250;
	speed = 0;

	starship = ((Phase *)game)->collisionables[ "starship" ];
	laser1 = ((Phase *)game)->collisionables[ "laser1" ];
	laser2 = ((Phase *)game)->collisionables[ "laser2" ];

	((Phase *)game)->scene->objects.push_back( rodObject );
	((Phase *)game)->scene->objects.push_back( bodyObject );
	((Phase *)game)->actuatorsToBeAdded.push_back( this );
	((SpaceGamePhase *)game)->enemies.push_back( this );

	return true;

}

void Enemy3Actuator::actuate( float dt, float time ) {

	if ( isDestroying ) {

		// Destruction
		if ( time > timeToDestroy ) ((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		else ( (SpaceGamePhase*)game )->explodeMeshRandomly( bodyObject->mesh, 0.05, dt );

	} else {

		// Animation

		if ( speed == 0 ) {
			speed = frand() * 240;
			targetY = frand() * 300 + 100;
			if ( targetY < bodyObject->pose->position.y ) speed = - speed;
		}

		float prevY = bodyObject->pose->position.y;
		bodyObject->pose->position.y += speed * dt;
		if ( ( targetY - prevY >= 0 ? 1 : - 1 ) != ( targetY - bodyObject->pose->position.y >= 0 ? 1 : - 1 ) ) speed = 0;

		bodyObject->pose->updateMatrix();

		float y = bodyObject->pose->position.y;
		for ( int32_t i = 0; i < 4; i ++ ) {
			Enemy3Ball *ball = &balls[ i ];
			ball->object->pose->position.y = y + 20;
			ball->object->pose->updateMatrix();
		}

	}

	// Collisions

	// Collision with the ship

	if ( spaceshipActuator ) {

		Object3D *spaceship = spaceshipActuator->spaceshipObject;

		// Collision with the rod
		if ( rodCollisionable->isPointInside( &spaceship->pose->position ) ) {

			// Collision sound
			if ( spaceshipActuator->velocity.length() > 20.0 ) {
				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "collision" ) ) ) {
					println( "Could not play collision" );
				}
			}

			// Reaction to rod collision
			spaceshipActuator->velocity.multiplyScalar( - 0.7 );
			spaceshipActuator->thrust = 0;
			spaceship->pose->position.dec( &spaceshipActuator->deltaPos );
			spaceshipActuator->deltaPos.cero();

		}

		// Collision with the body
		if ( ! isDestroying && bodyCollisionable->intersectsSphere( &spaceship->pose->position, spaceship->radius ) ) {

			// Reaction to body collision

			Vector3 normal;
			float dist = normal.sub( &spaceship->pose->position, &bodyObject->pose->position )->normalizeAndModulus();

			float proj = spaceshipActuator->velocity.dotProduct( &normal );
			Vector3 normalVel;
			normalVel.copy( &normal )->multiplyScalar( proj );
			Vector3 newVelocity;
			newVelocity.sub( &spaceshipActuator->velocity, &normalVel );
			spaceshipActuator->velocity.copy( &newVelocity );

			float penetration = spaceship->radius + bodyObject->radius - dist;
			if ( penetration > 0 ) {
				Vector3 penetrationVector;
				penetrationVector.copy( &normal )->multiplyScalar( penetration );
				spaceship->pose->position.inc( &penetrationVector );
				spaceship->pose->updateMatrix();
				spaceshipActuator->deltaPos.cero();
			}
		}


		// Balls

		if ( ! isDestroying ) {

			int numBallsDead = 0;
			for ( int32_t i = 0; i < 4; i ++ ) {

				Enemy3Ball *ball = &balls[ i ];

				if ( ball->health <= 0 ) {
					numBallsDead ++;
					continue;
				}

				// Collision with the lasers
				if ( laser1 ) {
					if ( laser1->intersectsSphere( &ball->object->pose->position, 10.0 ) ) {
						ball->health -= dt * 0.5;
						if ( ball->health <= 0 ) {
							if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ) ) ) {
								println( "Could not play bigExplosion" );
							}
							ball->health = 0;
							ball->object->visible = false;
							numBallsDead ++;
							spaceshipActuator->parameters.score += 2500;
						} else {
							float a = ball->health;
							ball->object->material->emission.set( 1 - a, a, a );
						}
					}
				}
				if ( laser2 ) {
					if ( laser2->intersectsSphere( &ball->object->pose->position, 10.0 ) ) {
						ball->health -= dt * 0.5;
						if ( ball->health <= 0 ) {
							if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ) ) ) {
								println( "Could not play bigExplosion" );
							}
							ball->health = 0;
							ball->object->visible = false;
							numBallsDead ++;
							spaceshipActuator->parameters.score += 2500;
						} else {
							float a = ball->health;
							ball->object->material->emission.set( 1 - a, a, a );
						}
					}
				}

			}

			if ( numBallsDead == 4 ) {

				// This enemy has been destroyed

				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ) ) ) {
					println( "Could not play bigExplosion" );
				}

				isDestroying = true;
				timeToDestroy = time + 8;

				rodObject->material->diffuse.set( .2, .2, .2 );
				rodObject->material->doubleSided = true;
				bodyObject->material->diffuse.set( .3, .3, .3 );
				bodyObject->material->doubleSided = true;

				spaceshipActuator->parameters.score += 2500;

			}
		}
	}
}

void Enemy3Actuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( rodObject );
	((Phase *)game)->scene->deleteObject( bodyObject );
	for ( int32_t i = 0; i < 4; i ++ ) {
		Enemy3Ball *ball = &balls[ i ];
		((Phase *)game)->scene->deleteObject( ball->object );
	}

	std::vector<Actuator *> &enemies = ((SpaceGamePhase *)game)->enemies;
	for ( uint32_t i = 0, n = enemies.size(); i < n; i ++ ) {
		if ( (enemies[ i ]) == this ) {
			enemies.erase( enemies.begin() + i );
			break;
		}
	}
}
