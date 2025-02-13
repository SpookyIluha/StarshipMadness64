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

#include "Phase1.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

#include "../actuators/PowerupActuator.h"
#include "../actuators/SpaceshipActuator.h"
#include "../actuators/EnemyActuator.h"
#include "../actuators/BannerActuator.h"
#include "../actuators/StarsActuator.h"
#include "../actuators/BulletActuator.h"
#include "../actuators/ScoreActuator.h"
#include "fixedPipeline/gl1/GL1Parallelepiped.h"
#include "../objects/Grid.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

using namespace ceres;

Phase1::Phase1() : SpaceGamePhase() {
}

Phase1::~Phase1() {
}

bool Phase1::initPhase( std::string &error ) {

	canPause = true;

	numWaves = 3;
	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = true;

	backgroundColor.set( 0.0, 0.03, 0.1, 0.0);

	// Camera

	camera = new Camera();
	camera->pose->position.set( 0, - 4, - 15 );

	// Scene

	scene = new GL1Scene();
	//scene->init();

	GL1ObjectUtils objectUtils;

	// Lighting

	setDefaultLight( scene );

	// Floor

	drawFloor = true;
	floorGrid = new Grid();
	floorGrid->pose = new Pose();
	floorGrid->color.set( 0, 1, 0 );
	zViewDistance = 650.0;
	floorGrid->zViewDistance = zViewDistance;
	floorGrid->squareLength = 90;
	floorGrid->squareLineWidth = 5;

	scene->objects = std::vector<ceres::Object3D *>();
	scene->objects.push_back( floorGrid );

	// Create buildings

	GL1Material *boxMaterial = new GL1Material();

	boxMaterial->diffuse.set( 0.7, 0.7, 0.7 );
	//boxMaterial->specular.set( 0.0, 0.0, 0.0 );
	boxMaterial->texture = NULL;
	boxMaterial->minZ = 15;
	boxMaterial->maxZ = 800;

	BoxCollisionable *boxCol = new BoxCollisionable();
	collisionables[ "buildings" ] = boxCol;

	Vector3 posarr[50];
	
	for ( int32_t i = 0; i < 50; i ++ ){
		bool faraway = false;
		while(!faraway){
			posarr[i].set( ( frand() - 0.5 ) * citySize, 0, ( frand() - 0.5 ) * citySize );
			faraway = true;
			for(int j = 0; j < i; j++){
				if(posarr[i].distanceTo(&posarr[j]) < 130) faraway = false;
			}
		}
	}

	for ( int32_t i = 0, n = 50; i < n; i ++ ) {

		GL1Parallelepiped *paralellepiped = new GL1Parallelepiped();
		//paralellepiped->pose->updateMatrix();
		paralellepiped->material = boxMaterial;
		paralellepiped->radius = 20.0;
		paralellepiped->material->texture = NULL;
		Vector3 pos = posarr[i];
		Vector3 p0( - frand() * 40 - 10, 0, - frand() * 40 - 10 );
		Vector3 p1( frand() * 40 + 10, frand() * 200 + 40, frand() * 40 + 10 );
		p0.inc( &pos );
		p1.inc( &pos );
		paralellepiped->box.expandByPoint( &p0 );
		paralellepiped->box.expandByPoint( &p1 );
		scene->objects.push_back( paralellepiped );
		buildings.push_back( paralellepiped );

		boxCol->boxes.push_back( &paralellepiped->box );

	}

	// Create spaceship
	spaceshipActuator = createSpaceship( true, error );
	if ( spaceshipActuator == NULL ) return false;

	// Add spaceship object to scene
	scene->objects.push_back( spaceshipActuator->spaceshipObject );

	// Position ship

	spaceshipActuator->spaceshipObject->pose->position.set( 0, 200, citySize * 0.5 * 1.5 );
	spaceshipActuator->spaceshipObject->pose->updateMatrix();

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

void Phase1::terminatePhase() {

	println( "- Phase1::terminatePhase" );

	delete camera;
	delete scene;

	delete collisionables[ "buildings" ];
	delete collisionables[ "starship" ];

	println( "- Phase1::terminatePhase (end)" );

}

void Phase1::timestep( float dt, float time ) {

	// Create powerups
	if ( allowCreatePowerup && time >= nextTimeCreatePowerup ) {

		allowCreatePowerup = false;
		nextTimeCreatePowerup = time + 30.0;

		std::string error;
		PowerupActuator *pu = createPowerup( -1, dt, time, error );
		if ( ! pu ) {
			println( error );
			return;
		}

		if ( ! placeObject( pu->object, collisionables[ "buildings" ], 2.0 * citySize * 0.7, 20.0, 80.0, Vector3() ) ) {
			actuatorsToBeDeleted.push_back( pu );
		}

	}

	// Update grid

	floorGrid->pose->position.copy( &spaceshipActuator->spaceshipObject->pose->position );
	floorGrid->pose->updateMatrix();

	if ( terminationRequested ) isTerminated = true;

}

void Phase1::createEnemiesWave( int32_t wave, float time ) {

	// Create enemies

	//int32_t numEnemies =  1;

	int32_t numEnemies =  6 + 2 * wave;
	for ( int32_t i = 0; i < numEnemies; i ++ ) {
		EnemyActuator *enemy1 = new EnemyActuator();
		enemy1->spaceshipActuator = spaceshipActuator;
		enemy1->game = this;
		std::string error;
		if ( ! enemy1->init( 0, 0, error ) ) return;
		enemy1->enemyObject->pose->position.set( ( frand() - 0.5 ) * citySize, ( frand() ) * citySize * 0.3, ( frand() - 0.5 ) * citySize - citySize );
	}

}
