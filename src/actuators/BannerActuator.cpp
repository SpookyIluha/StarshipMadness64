/*
 * Copyright © 2024 yomboprime https://codeberg.org/yombo
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

#include "BannerActuator.h"
#include "../objects/BannerMesh.h"
#include "../SpaceGamePhase.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Texture.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1Text3DScreen.h"

#include "console/console.h"

using namespace ceres;

BannerActuator::BannerActuator() :
	state( BANNER_IDLE ) {
}

BannerActuator::~BannerActuator(){
}

bool BannerActuator::init( float dt, float time, std::string &error ) {

	object = NULL;

	numWavesEmmited = 0;

	GL1Material *material1 = new GL1Material();
	material1->diffuse.set( 0.3, 0.1, 0.8 );
	//material1->specular.set( 0, 0, 0 );
	material1->illuminated = true;
	material1->depthTest = false;
	material1->minZ = 1;
	material1->maxZ = 400;

	GL1Material *material2 = new GL1Material();
	material2->diffuse.set( 0.9, 0.1, 0.2 );
	//material2->specular.set( 0, 0, 0 );
	material2->illuminated = true;
	material2->depthTest = false;
	material2->minZ = 1;
	material2->maxZ = 400;

	GL1ObjectUtils objectUtils;
	Vector3 bannerPosition( 0, 0, -4 );
	Vector3 digitPosition( 1.1, 0, -4 );

	waveTextObject = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/wave.stl" ), material1, 100.0, digitPosition, error, new BannerMesh() );
	if ( ! waveTextObject ) return false;
	waveTextObject->visible = false;
	waveTextObject->pose->scale = 2;
	((Phase *)game)->scene->objects.push_back( waveTextObject );

	gameOverTextObject = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/gameover.stl" ), material2, 100.0, bannerPosition, error, new BannerMesh() );
	if ( ! gameOverTextObject ) return false;
	gameOverTextObject->visible = false;
	gameOverTextObject->pose->scale = 2;
	((Phase *)game)->scene->objects.push_back( gameOverTextObject );

	for ( int digit = 0; digit < 10; digit ++ ) {
		GL1Mesh *digitObject = objectUtils.createObject(
			SPACESHIP_MADNESS_DIR + std::string( "stls/" ) +
			std::to_string( digit ) +
			std::string( ".stl" ),
			material1, 100.0, digitPosition, error, new BannerMesh() );
		if ( ! digitObject ) {
			return false;
		}
		digitsTextObjects[ digit ] = digitObject;
		digitObject->visible = false;
		digitObject->pose->scale = 2;
		((Phase *)game)->scene->objects.push_back( digitObject );
	}

	GL1Material *titleMaterial = new GL1Material();
	titleMaterial->diffuse.set( 1, 1, 1 );
	titleMaterial->texture = ((SpaceGamePhase *)game)->font1->texture;
	titleMaterial->transparent = true;
	titleMaterial->opacity = 1.0;
	titleMaterial->depthTest = false;
	titleMaterial->minZ = 1;
	titleMaterial->maxZ = 400;

	bonusText = new GL1Text3DScreen();
	bonusText->pose = new Pose();
	bonusText->pose->position.set( 55, 380, -4 );
	bonusText->pose->scale = 2;
	bonusText->pose->updateMatrix();
	bonusText->init( ((SpaceGamePhase *)game)->font1, titleMaterial, Vector3( 1.25, 1.25, 1 ) );
	bonusText->setString( "Time bonus: " );
	bonusText->visible = false;
	((Phase *)game)->scene->objects.push_back( bonusText );


	GL1Material *welldoneMaterial = new GL1Material();
	welldoneMaterial->illuminated = false;
	welldoneMaterial->diffuse.set( 1, 1, 1 );
	welldoneMaterial->doubleSided = true;
	welldoneMaterial->depthTest = false;
	welldoneMaterial->minZ = 1;
	welldoneMaterial->maxZ = 3000;
	GL1Texture *welldoneTexture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "textures/gradient1.sprite" ), error );
	if ( welldoneTexture == NULL ) return NULL;
	welldoneMaterial->texture = welldoneTexture;

	Vector3 welldonePosition( 0, 0, - 100 );
	welldoneText = (BannerMesh *)objectUtils.createPNCObject(
		SPACESHIP_MADNESS_DIR + std::string( "xyzuv/welldone.xyzuv" ),
		welldoneMaterial,
		10,
		welldonePosition,	
		error,
		new BannerMesh(),
		1.0,
		false
	);
	if ( ! welldoneText ) return false;
	welldoneText->visible = false;
	welldoneText->mesh->center();
	welldoneText->pose->scale = 0.75f;
	((Phase *)game)->scene->objects.push_back( welldoneText );

	((Phase *)game)->actuatorsToBeAdded.push_back( this );

	return true;

}

void BannerActuator::actuate( float dt, float time ) {

	SpaceGamePhase *currentPhase = (SpaceGamePhase *)game;
	Controller *controller = currentPhase->controller;
	if ( ! currentPhase->isTerminated ) {
		if ( currentPhase->numWaves == 0 && controller->a && time > 1 ) currentPhase->isTerminated = true;
		else {
			std::vector<Actuator *> &enemies = currentPhase->enemies;
			if ( enemies.size() == 0 && state == BANNER_IDLE ) {
				// Next enemies wave
				if ( numWavesEmmited == 0 ) showWave( 1, time );
				else showWellDone( time );
			}
		}
	}

	float dx = 0.0;
	float *verts = welldoneText->mesh->vertices;

	switch ( state ) {

		// Next wave sign
		case BANNER_SHOW_WAVE:
			if ( time > time0 + 3 ) {
				state = BANNER_IDLE;
				waveTextObject->visible = false;
				waveTextObject->material->diffuse.g = waveTextObject->material->diffuse.b = sin( M_PI * time ) * 0.5 + 0.5;
				for ( int digit = 0; digit < 10; digit ++ ) {
					digitsTextObjects[ digit ]->visible = false;
				}
				numWavesEmmited ++;
				currentPhase->createEnemiesWave( numWavesEmmited, time );
			}
			break;

		// Well done animation
		case BANNER_SHOW_WELL_DONE:
			if ( time > time0 + 5 ) {
				state = BANNER_IDLE;
				welldoneText->visible = false;
				bonusText->visible = false;
				if ( numWavesEmmited >= currentPhase->numWaves ) {
					currentPhase->isSucceeded = currentPhase->numWaves > 0;
					currentPhase->terminationRequested = true;
				} else showWave( numWavesEmmited + 1, time );
			} else if ( time > time0 + 3 ) {
				dx = - 0.5 * ( time - time0 - 3 ) * 280;
			} else if ( time > time0 + 2 ) {
				dx = 0;
			} else {
				dx = ( 1.0 - 0.5 * ( time - time0 ) ) * 280;
			}

			// Color cycling
			welldoneText->material->transX += 0.8 * dt;
			if(welldoneText->material->transX > 2) welldoneText->material->transX = 0;
			/*for ( int32_t i = 0, n = welldoneText->mesh->numVertices; i < n; i ++ ) {

				// Change U coordinate
				float u = verts[ i * 5 + 3 ];
				u += 0.8 * dt;
				if ( u >= 1 ) u --;
				verts[ i * 5 + 3 ] = u;

			}*/

			welldoneText->pose->position.x = dx;
			welldoneText->pose->updateMatrix();
			break;

		case BANNER_SHOW_GAME_OVER:
			if ( time > time0 + 0.8 && controller->a ) {
				currentPhase->terminationRequested = true;
			}
			break;

		case BANNER_IDLE:
		default:
			if ( spaceshipActuator->hasBeenHit ) {
				currentPhase->isSucceeded = false;
				showGameOver( time );
			}
			break;
	}

}

void BannerActuator::showWave( int32_t wave, float time ) {

	state = BANNER_SHOW_WAVE;
	time0 = time;

	waveTextObject->visible = true;
	gameOverTextObject->visible = false;

	for ( int digit = 0; digit < 10; digit ++ ) {
		digitsTextObjects[ digit ]->visible = false;
	}
	digitsTextObjects[ wave ]->visible = true;

}

void BannerActuator::showWellDone( float time ) {

	state = BANNER_SHOW_WELL_DONE;
	time0 = time;

	waveTextObject->visible = false;
	gameOverTextObject->visible = false;

	for ( int digit = 0; digit < 10; digit ++ ) {
		digitsTextObjects[ digit ]->visible = false;
	}

	welldoneText->visible = true;

	bonusText->visible = true;
	int32_t timeBonus  = floor( maxim( 0, 10000 - time0 * 100 ) / 500 ) * 500;
	bonusText->setString( std::string( "Time bonus: " ) + std::to_string( timeBonus ) );
	spaceshipActuator->parameters.score += timeBonus;
	((SpaceGamePhase *)game)->scoreActuator->screenScore = spaceshipActuator->parameters.score;

}

void BannerActuator::showGameOver( float time ) {

	state = BANNER_SHOW_GAME_OVER;
	time0 = time;

	waveTextObject->visible = false;
	gameOverTextObject->visible = true;

	for ( int digit = 0; digit < 10; digit ++ ) {
		digitsTextObjects[ digit ]->visible = false;
	}

}

void BannerActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( waveTextObject );
	((Phase *)game)->actuatorsToBeDeleted.push_back( this );

}
