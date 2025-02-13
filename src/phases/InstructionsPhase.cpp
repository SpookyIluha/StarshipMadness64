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

#include "InstructionsPhase.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

using namespace ceres;

InstructionsPhase::InstructionsPhase() : SpaceGamePhase() {
	camera = NULL;
	scenes[0] = NULL;
	scenes[1] = NULL;
	scenes[2] = NULL;
}

InstructionsPhase::~InstructionsPhase() {
}

bool InstructionsPhase::initPhase( std::string &error ) {

	canPause = false;

	numWaves = 0;
	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = false;
	drawFloor = false;
	spaceshipActuator = NULL;

	scene = NULL;

	backgroundColor.set( 0.1, 0, 0, 0);

	// Camera

	camera = new Camera();

	// Scenes

	GL1ObjectUtils objectUtils;

	// *************************************************************
	// Scene 0
	// *************************************************************

	GL1Scene *scene0 = new GL1Scene();
	//scene0->init();

	setDefaultLight( scene0 );

	// Create spaceship

	spaceshipActuator = createSpaceship( false, error );
	if ( spaceshipActuator == NULL ) return false;

	scoreActuator = NULL;

	// Add spaceship object to scene
	scene0->objects.push_back( spaceshipActuator->spaceshipObject );

	// Position ship

	spaceshipActuator->spaceshipObject->pose->position.set( - 10, -10, - 30 );
	spaceshipActuator->spaceshipObject->pose->updateMatrix();

	// Text

	GL1Material *textMaterial = new GL1Material();
	textMaterial->illuminated = false;
	textMaterial->diffuse.set( 1, 1, 1 );
	textMaterial->texture = font1->texture;
	textMaterial->transparent = true;
	textMaterial->opacity = 1.0;
	textMaterial->depthTest = false;

	GL1Text3D* text = new GL1Text3D();
	text->pose = new Pose();
	text->pose->position.set( 20, 60, - 8 );
	text->pose->updateMatrix();
	text->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text->setString( "Ship controls\n\nAccelerate:      D-Up C-Up\nGo backwards:      D-Down C-Down\nPitch, yaw:      Main Stick\nRoll:      D-Left C-Left, D-Right C-Right\nStabilize:      L R Triggers\nLaser:      Z Trigger\n\n\n\n\n                                                  1/3" );
	scene0->objects.push_back( text );

	// *************************************************************
	// Scene 2
	// *************************************************************

	GL1Scene *scene2 = new GL1Scene();
	scene2->init();

	GL1Light *light2 = (GL1Light *)( scene2->lights[ 0 ] );

	light2->enabled = true;
	light2->isSpot = false;
	light2->position.set( 0.0, 0.0, 1.0 );
	light2->ambient.set( 0, 0, 0 );
	light2->diffuse.set( 1, 1, 1 );
	//light2->specular.set( 0.1, 0.1, 0.1 );
	light2->updateParams();

	// Text

	GL1Text3D *text2 = new GL1Text3D();
	text2->pose = new Pose();
	text2->pose->position.set( 20, 60, - 8 );
	text2->pose->updateMatrix();
	text2->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text2->setString( "Get Power-ups!!\n\n\nMax shield powerup\n\nRegenerate shield powerup\n\nMax energy powerup\n\nRegenerate energy powerup\n\n\n                                                  2/3" );
	scene2->objects.push_back( text2 );

	// Powerups

	powerups[ 0 ] = createPowerup( 2, 0, 0, error );
	if ( ! powerups[ 0 ] ) return NULL;
	powerups[ 1 ] = createPowerup( 3, 0, 0, error );
	if ( ! powerups[ 1 ] ) return NULL;
	powerups[ 2 ] = createPowerup( 0, 0, 0, error );
	if ( ! powerups[ 2 ] ) return NULL;
	powerups[ 3 ] = createPowerup( 1, 0, 0, error );
	if ( ! powerups[ 3 ] ) return NULL;
	float y = - 30 + 3 * 25;
	powerups[ 0 ]->object->pose->position.set( 100, y, - 150 );
	powerups[ 0 ]->object->pose->scale = 0.3f;
	y-= 25;
	powerups[ 1 ]->object->pose->position.set( 100, y, - 150 );
	powerups[ 1 ]->object->pose->scale = 0.3f;
	y-= 25;
	powerups[ 2 ]->object->pose->position.set( 100, y, - 150 );
	powerups[ 2 ]->object->pose->scale = 0.3f;
	y-= 25;
	powerups[ 3 ]->object->pose->position.set( 100, y, - 150 );
	powerups[ 3 ]->object->pose->scale = 0.3f;

	for ( int32_t i = 0; i < 4; i ++ ) {
		powerups[ i ]->object->pose->updateMatrix();
		scene2->objects.push_back( powerups[ i ]->object );
	}

	// *************************************************************
	// Scene 3
	// *************************************************************

	GL1Scene *scene3 = new GL1Scene();
	scene3->init();

	GL1Light *light3 = (GL1Light *)( scene3->lights[ 0 ] );

	light3->enabled = true;
	light3->isSpot = false;
	light3->position.set( 0.0, 0.0, 1.0 );
	light3->ambient.set( 0, 0, 0 );
	light3->diffuse.set( 1, 1, 1 );
	//light3->specular.set( 0.1, 0.1, 0.1 );
	light3->updateParams();

	// Text

	GL1Text3D *text3 = new GL1Text3D();
	text3->pose = new Pose();
	text3->pose->position.set( 20, 60, - 8 );
	text3->pose->updateMatrix();
	text3->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text3->setString( "Defend your cyberplanet from the waves of\ninvader aliens.\n\nDestroy all enemies of each wave.\n\n\n\n\n\n\n\n\n                                                   3/3" );
	scene3->objects.push_back( text3 );

	EnemyActuator *enemy1 = new EnemyActuator();
	enemy1->game = this;
	enemy1->spaceshipActuator = NULL;
	if ( ! enemy1->init( 0, 0, error ) ) return false;
	enemy1->enabled = false;
	enemy1->enemyObject->pose->position.set( - 40, - 20, - 100 );
	enemy1->enemyObject->pose->updateMatrix();
	scene3->objects.push_back( enemy1->enemyObject );

	EnemyActuator *enemy2 = new EnemyActuator();
	enemy2->game = this;
	enemy2->enemyType = 2;
	enemy2->spaceshipActuator = NULL;
	if ( ! enemy2->init( 0, 0, error ) ) return false;
	enemy2->enabled = false;
	enemy2->enemyObject->pose->position.set( 40, - 20, - 100 );
	enemy2->enemyObject->pose->updateMatrix();
	scene3->objects.push_back( enemy2->enemyObject );


	// Initial scene

	scenes[ 0 ] = scene0;
	scenes[ 1 ] = scene2;
	scenes[ 2 ] = scene3;

	scene = scene0;
	currentSceneIndex = 0;

	prevPressedTime = 0;

	return true;

}

void InstructionsPhase::terminatePhase() {
	if(camera != NULL)
		delete camera;
	for ( uint32_t i = 0; i < NUM_SCENES; i ++ ) {
		if(scenes[ i ] != NULL)
		delete scenes[ i ];
	}

}

void InstructionsPhase::timestep( float dt, float time ) {

	if ( controller->a ) {
		if ( time > prevPressedTime + 0.3 ) {
			currentSceneIndex ++;
			sound->playAudio( "menu1" );
			if ( currentSceneIndex >= NUM_SCENES ) isTerminated = true;
			else {

				scene = scenes[ currentSceneIndex ];

				switch ( currentSceneIndex ) {
					case 1:
						backgroundColor.set( 0, 0.1, 0, 0);
						break;
					case 2:
						backgroundColor.set( 0, 0, 0.1, 0);
						break;
					default:
						break;
				}
			}
		}
		prevPressedTime = time;
	}

	Vector3 axis( 0, 1, 0 );

	switch ( currentSceneIndex ) {
		case 0:
			spaceshipActuator->spaceshipObject->pose->rotation.setFromAxisAngle( &axis, time * 0.8 );
			spaceshipActuator->spaceshipObject->pose->updateMatrix();
			break;
		case 1:
			powerups[ 0 ]->object->pose->rotation.setFromAxisAngle( &axis, 2 * time );
			powerups[ 1 ]->object->pose->rotation.setFromAxisAngle( &axis, 2 * time + 1 );
			powerups[ 2 ]->object->pose->rotation.setFromAxisAngle( &axis, 2 * time + 2 );
			powerups[ 3 ]->object->pose->rotation.setFromAxisAngle( &axis, 2 * time + 3 );
			for ( int32_t i = 0; i < 4; i ++ ) powerups[ i ]->object->pose->updateMatrix();
			break;
		case 2:
			break;
		default:
			break;
	}

}
