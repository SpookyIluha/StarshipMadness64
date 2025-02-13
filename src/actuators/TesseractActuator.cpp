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

#include "TesseractActuator.h"
#include "../SpaceGamePhase.h"
#include "../objects/Tesseract.h"
#include "../collisionGame/TesseractCollisionable.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "console/console.h"

using namespace ceres;

TesseractActuator::TesseractActuator() {
}

TesseractActuator::~TesseractActuator(){
}

bool TesseractActuator::init( float dt, float time, std::string &error ) {

	isDestroying = false;;
	timeToDestroy = 0;
	coreBallHealth = 1.0;

	GL1Material *tesseractMaterial = new GL1Material();
	tesseractMaterial->diffuse.set( 0.8, 0.8, 0.8 );
	//tesseractMaterial->specular.set( 0.0, 0.0, 0.0 );
	tesseractMaterial->doubleSided = false;
	tesseractMaterial->minZ = 15;
	tesseractMaterial->maxZ = 400;

	GL1Material *coreMaterial = new GL1Material();
	coreMaterial->diffuse.set( 0, 0.5, 0.8 );
	coreMaterial->minZ = 15;
	coreMaterial->maxZ = 400;
	//coreMaterial->specular.set( 0.0, 0.0, 0.0 );

	tesseractObject = new Tesseract();
	tesseractObject->material = tesseractMaterial;
	tesseractObject->pose = new Pose();
	tesseractObject->pose->position.y  = 125;
	tesseractObject->pose->updateMatrix();
	for ( int32_t i = 0, n = tesseractObject->points.size(); i < n; i ++ ) tesseractObject->points[ i ].multiplyScalar( 125 );
	tesseractObject->size = 125;
	tesseractObject->tesframe = 0;
	tesseractObject->block = NULL;

	GL1ObjectUtils objectUtils;
	Vector3 corePos( 125, 125, 125 );
	tesseractCoreObject = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/ball.stl" ), coreMaterial, 100, corePos, error, new GL1Mesh(), 250.0 );
	((Phase *)game)->scene->objects.push_back( tesseractCoreObject );

	object = tesseractCoreObject;

	starship = ((Phase *)game)->collisionables[ "starship" ];
	laser1 = ((Phase *)game)->collisionables[ "laser1" ];
	laser2 = ((Phase *)game)->collisionables[ "laser2" ];

	tesseractCollisionable = new TesseractCollisionable();
	tesseractCollisionable->tesseract = tesseractObject;
	tesseractCollisionable->thickness = tesseractObject->thickness;

	((Phase *)game)->scene->objects.push_back( tesseractObject );
	((Phase *)game)->actuatorsToBeAdded.push_back( this );
	((SpaceGamePhase *)game)->enemies.push_back( this );

	return true;

}

void TesseractActuator::actuate( float dt, float time ) {

	if ( isDestroying ) {
		//debugf("Tesseract: %.2f > %.2f\n", time, timeToDestroy);
		// Destruction
		if ( time > timeToDestroy ) {
			tesseractCoreObject->visible = false;
			((Phase *)game)->actuatorsToBeDeleted.push_back( this );
		} else ( (SpaceGamePhase*)game )->explodeMeshRandomly( tesseractCoreObject->mesh, 0.1, dt );

	}

	// Transform points
	Vector3 axis( 0.123, 1.234, 3.456 );
	axis.normalize();
	tesseractObject->pose0.position.set( 100 * sin( 0 * 0.5 ), 0, 100 * cos( 0 * 0.5 ) );
	tesseractObject->pose0.rotation.setFromAxisAngle( &axis, 0 * 0.3 );
	tesseractObject->pose0.updateMatrix();
	tesseractObject->pose1.position.set( sin( 0 * 0.5 + 1 ), 0, cos( 0 * 0.5 + 1 ) );
	tesseractObject->pose1.rotation.setFromAxisAngle( &axis, 0 * 0.3 );
	tesseractObject->pose1.updateMatrix();

	tesseractObject->transformPoints();

	Vector3 v;
	v.set( tesseractObject->size, tesseractObject->size, tesseractObject->size )->multiplyScalar( 0.5 );
	Vector3 v2;
	v2 = tesseractObject->pose0.matrix.vector3XMatrix4( &v );
	tesseractCoreObject->pose->position = tesseractObject->pose->matrix.vector3XMatrix4( &v2 );
	tesseractCoreObject->pose->updateMatrix();

	// Collisions

	// Collision with the ship

	if ( spaceshipActuator ) {

		Object3D *spaceship = spaceshipActuator->spaceshipObject;

		// Collision with the ship
		if ( tesseractCollisionable->isPointInside( &spaceship->pose->position ) ) {

			// Collision sound
			if ( spaceshipActuator->velocity.length() > 20.0 ) {
				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "collision" ) ) ) {
					println( "Could not play collision" );
				}
			}

			// Reaction to tesseract collision
			spaceshipActuator->thrust = 0;
			spaceship->pose->position.dec( &spaceshipActuator->deltaPos );
			spaceshipActuator->deltaPos.cero();

		}

		// Collision with the lasers
		float intersectionParam;
		Vector3 intersectionNormal;
		if ( tesseractCollisionable->castRay( &spaceshipActuator->laser1->beginPoint, &spaceshipActuator->laser1->ray, &intersectionParam, &intersectionNormal ) ) {
			spaceshipActuator->laser1->ray.multiplyScalar( intersectionParam );
		}
		if ( tesseractCollisionable->castRay( &spaceshipActuator->laser2->beginPoint, &spaceshipActuator->laser2->ray, &intersectionParam, &intersectionNormal ) ) {
			spaceshipActuator->laser2->ray.multiplyScalar( intersectionParam );
		}

		// Collision of core ball with the lasers

		float prevHealth = coreBallHealth;
		if ( laser1 ) {
			if ( laser1->intersectsSphere( &tesseractCoreObject->pose->position, 25.0 ) && spaceshipActuator->laser1->visible ) {
				coreBallHealth -= dt * 0.15;
				if ( coreBallHealth <= 0 ) {
					if ( prevHealth > 0 ) {
						if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ) ) ) {
							println( "Could not play bigExplosion" );
						}
						spaceshipActuator->parameters.score += 5000;
						coreBallHealth = 0;
						isDestroying = true;
						timeToDestroy = time + 3;
						name = "x";
					}
				} else {
					float a = coreBallHealth;
					tesseractCoreObject->material->emission.set( 1 - a, a, a );
				}
			}
		}
		if ( laser2 ) {
			if ( laser2->intersectsSphere( &tesseractCoreObject->pose->position, 25.0 ) ) {
				coreBallHealth -= dt * 0.15;
				if ( coreBallHealth <= 0 ) {
					if ( prevHealth > 0 ) {
						if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "bigExplosion" ) ) ) {
							println( "Could not play bigExplosion" );
						}
						spaceshipActuator->parameters.score += 5000;
						coreBallHealth = 0;
						isDestroying = true;
						timeToDestroy = time + 3;
						name = "x";
					}
				} else {
					float a = coreBallHealth;
					tesseractCoreObject->material->emission.set( 1 - a, a, a );
				}
			}
		}
	}
}

void TesseractActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( tesseractObject );

	std::vector<Actuator *> &enemies = ((SpaceGamePhase *)game)->enemies;
	for ( uint32_t i = 0, n = enemies.size(); i < n; i ++ ) {
		if ( (enemies[ i ]) == this ) {
			enemies.erase( enemies.begin() + i );
			break;
		}
	}
}
