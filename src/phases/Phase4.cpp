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

#include "Phase4.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

#include "../actuators/PowerupActuator.h"
#include "../actuators/SpaceshipActuator.h"
#include "../actuators/TesseractActuator.h"
#include "../actuators/BannerActuator.h"
#include "../actuators/StarsActuator.h"
#include "fixedPipeline/gl1/GL1Parallelepiped.h"

#include "console/console.h"
#include "fixedPipeline/gl1/GL1Light.h"

#include "../SpaceshipMadnessConfig.h"

using namespace ceres;

Phase4::Phase4() : SpaceGamePhase() {
}

Phase4::~Phase4() {
}

bool Phase4::initPhase( std::string &error ) {

	canPause = true;

	numWaves = 3;

	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = true;

	backgroundColor.set( 0.2, 0.25, 0.25, 0 );

	// Camera

	camera = new Camera();
	camera->pose->position.set( 0, - 4, - 15 );

	// Scene

	scene = new GL1Scene();
	//scene->init();

	// Lights

	setDefaultLight( scene );

	// Floor and ceiling

	drawFloor = true;
	drawCeiling = true;
	floorGrid = new Grid();
	floorGrid->pose = new Pose();
	floorGrid->color.set( 0.75, 0.75, 0.75 );
	ceilingGrid = new Grid();
	ceilingGrid->color.set( 0.9, 0.9, 0.9 );
	ceilingGrid->pose = new Pose();
	ceilingHeight = 200.0;
	ceilingGrid->height = ceilingHeight;
	zViewDistance = 400.0;
	floorGrid->zViewDistance = zViewDistance;
	ceilingGrid->zViewDistance = zViewDistance;
	squareLength = 60;
	squareLineWidth = 5;

	scene->objects.push_back( floorGrid );
	scene->objects.push_back( ceilingGrid );

	// Create spaceship

	spaceshipActuator = createSpaceship( true, error );
	if ( spaceshipActuator == NULL ) return false;

	// Add spaceship object to scene
	scene->objects.push_back( spaceshipActuator->spaceshipObject );

	// Position ship

	spaceshipActuator->spaceshipObject->pose->position.set( 0, 100, 300 );
	spaceshipActuator->spaceshipObject->pose->updateMatrix();

	// Create random object

	GL1ObjectUtils objectUtils;

	GL1Material *material = new GL1Material();
	material->diffuse.set( 0.6, 0.5, 0.25 );
	material->specular.set( 0, 0, 0 );
	Vector3 boatPos( 0, 100, - 500 );
	boat = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "models/boat/boat.stl" ), material, 10, boatPos, error, new GL1Mesh(), 30 );
	if ( ! boat ) return false;
	Vector3 axis( 1, 2, 3 );
	axis.normalize();
	boat->pose->rotation.setFromAxisAngle( &axis, 1.2 );
	boat->pose->updateMatrix();
	scene->objects.push_back( boat );

	boatCollisionable = new RadiusCollisionable();
	boatCollisionable->radius = 60;
	boatCollisionable->position = &boat->pose->position;

	// Another one

	GL1Material *material2 = new GL1Material();
	material2->diffuse.set( 0.1, 0.75, 0.9 );
	material2->specular.set( 0, 0, 0 );
	Vector3 elephantPos( -10, 80, - 900 );
	axis.set( 2, -1, 0 )->normalize();
	elephant = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "models/elephant/Elephant.stl" ), material2, 10, elephantPos, error, new GL1Mesh(), 30 );
	elephant->pose->rotation.setFromAxisAngle( &axis, 0.8 );
	elephant->pose->updateMatrix();
	if ( ! elephant ) return false;
	scene->objects.push_back( elephant );

	elephantCollisionable = new RadiusCollisionable();
	elephantCollisionable->radius = 30;
	elephantCollisionable->position = &elephant->pose->position;

	currentROI.cero();

	// Create stars actuator

	StarsActuator *starsActuator = new StarsActuator();
	starsActuator->game = this;
	starsActuator->spaceshipActuator = spaceshipActuator;
	if ( ! starsActuator->init( 0, 0, error ) ) return false;

	// Create score banner
	scoreActuator = new ScoreActuator();
	scoreActuator->game = this;
	scoreActuator->spaceshipActuator = spaceshipActuator;
	if ( ! scoreActuator->init( 0, 0, error ) ) return false;

	// Create Banner actuator (enemies waves gameplay generator)

	bannerActuator = new BannerActuator();
	bannerActuator->game = this;
	bannerActuator->spaceshipActuator = spaceshipActuator;
	bannerActuator->scoreActuator = scoreActuator;
	if ( ! bannerActuator->init( 0.0, 0.0, error ) ) return false;

	return true;

}

void Phase4::terminatePhase() {

	delete camera;
	delete scene;

//	delete collisionables[ "starship" ];

}

void Phase4::timestep( float dt, float time ) {

	// Create powerups
	if ( allowCreatePowerup && time >= nextTimeCreatePowerup ) {

		allowCreatePowerup = false;
		nextTimeCreatePowerup = time + 25.0;

		std::string error;
		PowerupActuator *pu = createPowerup( -1, dt, time, error );
		if ( ! pu ) {
			println( error );
			return;
		}

		placeObject( pu->object, NULL, 700, 20, 180, currentROI );

	}

	if ( spaceshipActuator ) {
		Object3D *spaceship = spaceshipActuator->spaceshipObject;
		if ( elephantCollisionable->isPointInside( &spaceshipActuator->spaceshipObject->pose->position ) ) {

			// Collision sound
			if ( spaceshipActuator->velocity.length() > 20.0 ) {
				if ( ! sound->playAudio( std::string( "collision" ) ) ) {
					println( "Could not play collision" );
				}
			}

			spaceshipActuator->thrust = 0;
			spaceship->pose->position.dec( &spaceshipActuator->deltaPos );
			spaceshipActuator->deltaPos.cero();

		}
		if ( boatCollisionable->isPointInside( &spaceshipActuator->spaceshipObject->pose->position ) ) {

			// Collision sound
			if ( spaceshipActuator->velocity.length() > 20.0 ) {
				if ( ! sound->playAudio( std::string( "collision" ) ) ) {
					println( "Could not play collision" );
				}
			}

			spaceshipActuator->thrust = 0;
			spaceship->pose->position.dec( &spaceshipActuator->deltaPos );
			spaceshipActuator->deltaPos.cero();

		}
	}

	// Update grids

	floorGrid->pose->position.copy( &spaceshipActuator->spaceshipObject->pose->position );
	ceilingGrid->pose->position.copy( &spaceshipActuator->spaceshipObject->pose->position );
	floorGrid->pose->updateMatrix();
	ceilingGrid->pose->updateMatrix();

	if ( terminationRequested ) isTerminated = true;

}


void Phase4::createEnemiesWave( int32_t wave, float time ) {

	// Create enemies

	// Flies

	int32_t numEnemies = 6;
	for ( int32_t i = 0; i < numEnemies; i ++ ) {
		EnemyActuator *enemy1 = new EnemyActuator();
		enemy1->enemyType = 2;
		enemy1->spaceshipActuator = spaceshipActuator;
		enemy1->game = this;
		std::string error;
		if ( ! enemy1->init( 0, 0, error ) ) return;
		enemy1->enemyObject->pose->position.set( ( frand() - 0.5 ) * 500, 0, ( frand() - 0.5 ) * 500 );
		enemy1->enemyObject->pose->position.inc( &currentROI );
		enemy1->enemyObject->pose->updateMatrix();

	}

}
