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

#include "Phase2.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

#include "../actuators/PowerupActuator.h"
#include "../actuators/SpaceshipActuator.h"
#include "../actuators/Enemy3Actuator.h"
#include "../actuators/BannerActuator.h"
#include "../actuators/StarsActuator.h"
#include "fixedPipeline/gl1/GL1Parallelepiped.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

#include "../SpaceshipMadnessConfig.h"

using namespace ceres;

Phase2::Phase2() : SpaceGamePhase() {
}

Phase2::~Phase2() {
}

bool Phase2::initPhase( std::string &error ) {

	canPause = true;

	numWaves = 3;

	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = true;

	backgroundColor.set( 0.0, 0.07, 0.12, 0.0);

	// Camera

	camera = new Camera();
	camera->pose->position.set( 0, - 4, - 15 );

	// Scene

	scene = new GL1Scene();
	//scene->init();

	// Lighting

	setDefaultLight( scene );

	// Floor and ceiling

	drawFloor = true;
	drawCeiling = true;
	floorGrid = new Grid();
	floorGrid->pose = new Pose();
	floorGrid->color.set( 1, 0.1, 0 );
	ceilingGrid = new Grid();
	ceilingGrid->color.set( 1, 0.5, 0 );
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

	// Arrow

	GL1Material *arrowMaterial = new GL1Material();
	arrowMaterial->diffuse.set( 0.8, 0.8, 0.8 );
	arrowMaterial->minZ = 1;
	arrowMaterial->maxZ = 800;
	//arrowMaterial->specular.set( 0.0, 0.0, 0.0 );

	GL1ObjectUtils objectUtils;
	Vector3 arrowPosition( 0, 100, -45 );
	arrow = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/arrow.stl" ), arrowMaterial, 4.0, arrowPosition, error, new GL1Mesh(), 0.15 );
	arrow->pose->scale = 2;
	if ( ! arrow ) return false;
	scene->objects.push_back( arrow );

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

void Phase2::terminatePhase() {

	delete camera;
	delete scene;
	//delete collisionables[ "starship" ];

}

void Phase2::timestep( float dt, float time ) {

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

	// Update arrow (radar)

	Quaternion compass;
	Vector3 roiDirection;
	roiDirection.sub( &currentROI, &spaceshipActuator->spaceshipObject->pose->position );
	float distance = roiDirection.normalizeAndModulus();
	if ( distance < 500 ) arrow->visible = false;
	else {
		Vector3 roiDirectionLocal = spaceshipActuator->spaceshipObject->pose->matrix.vector3FreeXMatrix4Inverse( &roiDirection );
		float angle = atan( - roiDirectionLocal.x / abs( roiDirectionLocal.z ) );
		if ( roiDirectionLocal.z > 0 ) angle = M_PI - angle;
		Vector3 yAxis( 0, 1, 0 );
		compass.setFromAxisAngle( &yAxis, angle );
		Matrix4 m;
		m.setRotationFromQuaternion( &compass );
		m.setTraslation( 0, 1.5, - 4 );
		arrow->pose->matrix.multiply( &camera->pose->matrix, &m );
		arrow->visible = true;
	}

	if ( terminationRequested ) isTerminated = true;

}


void Phase2::createEnemiesWave( int32_t wave, float time ) {

	// Create enemies

	// Ball on axis

	Enemy3Actuator *enemy3 = new Enemy3Actuator();
	enemy3->spaceshipActuator = spaceshipActuator;
	enemy3->game = this;
	float d = 1800;
	enemy3->initialPosition.set( 2.0 * ( frand() - 0.5 ) * d, 250, 2.0 * ( frand() - 0.5 ) * d );
	currentROI.y = 0;
	enemy3->initialPosition.inc( &currentROI );
	std::string error;
	if ( ! enemy3->init( 0, 0, error ) ) return;
	currentROI.copy( &enemy3->bodyObject->pose->position );

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
