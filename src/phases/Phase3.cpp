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

#include "Phase3.h"

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
#include <fixedPipeline/gl1/GL1Light.h>

using namespace ceres;

Phase3::Phase3() : SpaceGamePhase() {
}

Phase3::~Phase3() {
}

bool Phase3::initPhase( std::string &error ) {

	canPause = true;

	numWaves = 3;

	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = true;

	backgroundColor.set( 0.15, 0.15, 0.15, 0 );

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
	floorGrid->color.set( 0, 0, 0.8 );
	ceilingGrid = new Grid();
	ceilingGrid->color.set( 0, 0.5, 0.8 );
	ceilingGrid->pose = new Pose();
	ceilingHeight = 500.0;
	ceilingGrid->height = ceilingHeight;
	zViewDistance = 500.0;
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

void Phase3::terminatePhase() {

	delete camera;
	delete scene;
	//delete collisionables[ "starship" ];

}

void Phase3::timestep( float dt, float time ) {

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

		placeObject( pu->object, NULL, 700, 20, 400, currentROI );

	}

	// Update grids

	floorGrid->pose->position.copy( &spaceshipActuator->spaceshipObject->pose->position );
	ceilingGrid->pose->position.copy( &spaceshipActuator->spaceshipObject->pose->position );
	floorGrid->pose->updateMatrix();
	ceilingGrid->pose->updateMatrix();

	if ( terminationRequested ) isTerminated = true;

}


void Phase3::createEnemiesWave( int32_t wave, float time ) {

	// Create enemies

	// Tesseract

	TesseractActuator *tesseract = new TesseractActuator();
	tesseract->spaceshipActuator = spaceshipActuator;
	tesseract->game = this;
	currentROI.y = 0;
	std::string error;
	if ( ! tesseract->init( 0, 0, error ) ) return;
	float d = 1000;
	tesseract->tesseractCoreObject->pose->position.set( 2.0 * ( frand() - 0.5 ) * d, 125, - frand() * d );
	tesseract->tesseractCoreObject->pose->updateMatrix();
	currentROI.copy( &tesseract->tesseractObject->pose->position );

	// Flies

	int32_t numEnemies = 4;
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
