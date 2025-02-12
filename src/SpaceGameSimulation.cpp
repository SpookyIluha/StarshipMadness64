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

#include "SpaceGameSimulation.h"

#include <dc/video.h>

#include "phases/StartingPhase.h"
#include "phases/Phase1.h"
#include "phases/Phase2.h"
#include "phases/Phase3.h"
#include "phases/Phase4.h"
#include "phases/EndingPhase.h"
#include "phases/InstructionsPhase.h"
#include "phases/AboutPhase.h"
#include "math3d/importexport/STLMeshImporter.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Scene.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Texture.h"

using namespace ceres;

/*

	Phases (levels):
	0: Starting screen
	1: Instructions
	2: About
	3: Phase 1
	4: Phase 2
	5: Phase 4
	6: Phase 3
	7: Ending screen

*/

#define STARTING_TITLE_MUSIC_PATH ( "/cd/starship_madness_music/lasflores.wav" )
#define ENDING_TITLE_MUSIC_PATH ( "/cd/starship_madness_music/elviciodelsilicio.wav" )

std::string SpaceGameSimulation::getTitle() {

	return std::string( "'Starship Madness' game by Yomboprime. Copyright 2024 GPL2.0 license." );

}

bool SpaceGameSimulation::init( std::string& error ) {

	// Load audios

	sound = new Sound();

	if ( ! loadAudios( error ) ) return false;

	// Start random seed and set time to 0

	frandseed( 12345 );
	theTime = 0.0;

	// Get controller

	mapleController = maple_enum_type( 0, MAPLE_FUNC_CONTROLLER );

	// Init GL pipeline

	if ( ! pipeline.init( error ) ) return false;

	// Create main text font

	font1 = createFont1( error );
	if ( font1 == NULL ) return false;

	// Create and init the first phase (startup screen)

	currentPhaseIndex = 0;
	currentPhase = createPhase( currentPhaseIndex );

	if ( ! currentPhase->initPhase( error ) ) return false;

	selectedPhaseIndex = -1;

	// Create menu
	menu = new Menu();
	menu->createMenus( font1, sound );
	menu->controller = &controller;

	// Create screensaver

	screenSaverCamera = new Camera();
	screenSaverScene = new Scene();
	screenSaver = false;
	timeLastKeypress = 0;
	screenSaverDirection.set( 1, - 1, 0 );
	screenSaverPos.set( 0.54321, 0, - 8 );

	GL1Material *textMaterial = new GL1Material();
	textMaterial->illuminated = false;
	textMaterial->diffuse.set( 1, 1, 1 );
	textMaterial->texture = font1->texture;
	textMaterial->transparent = true;
	textMaterial->opacity = 1.0;

	screenSaverText = new GL1Text3D();
	screenSaverText->pose = new Pose();
	screenSaverText->init( font1, textMaterial, Vector3( 1, 1, 1 ) );
	screenSaverText->setString( "Starship madness" );
	screenSaverScene->objects.push_back( screenSaverText );

	// Start music
	if ( ! sound->playMusic( STARTING_TITLE_MUSIC_PATH ) ) {
		println( "Could not start the music" );
	}

	return true;

}

Simulation::SimulationStepResult SpaceGameSimulation::timestep( scalar dt, scalar elapsedRealTime ) {

	// Read controller

	controllerState = (cont_state_t *)maple_dev_status( mapleController );
	if( ! controllerState ) {
		println( "Error reading controller" );
		return SIM_TERMINATE;
	}
	controller.x1 = controllerState->joyx / 128.0;
	controller.y1 = controllerState->joyy / 128.0;
	controller.x2 = controllerState->joy2x / 128.0;
	controller.y2 = controllerState->joy2y / 128.0;
	controller.a = controllerState->a;
	controller.b = controllerState->b;
	controller.c = controllerState->x;
	controller.d = controllerState->y;
	controller.up = controllerState->dpad_up;
	controller.down = controllerState->dpad_down;
	controller.left = controllerState->dpad_left;
	controller.right = controllerState->dpad_right;
	controller.leftTrigger = controllerState->ltrig / 255.0;
	controller.rightTrigger = controllerState->rtrig / 255.0;
	controller.start = controllerState->start;

	// Quick exit from program
	if ( controller.start && controller.a ) {
		 return SIM_TERMINATE;
	}

	// Screen saver
	if ( screenSaver ) {
		if ( controllerState->buttons ) {
			screenSaver = false;
			timeLastKeypress = elapsedRealTime;
		}
	} else {
		if ( ! controllerState->buttons ) {
			if ( elapsedRealTime > timeLastKeypress + 300 ) screenSaver = true;
		} else timeLastKeypress = elapsedRealTime;
	}

	// Hide / show menu
	if ( controller.start && elapsedRealTime > menu->nextTimePressed ) {

		menu->nextTimePressed = elapsedRealTime + 0.3;
		if ( menu->isShown ) menu->hide();
		else menu->show( currentPhaseIndex > 2 );
	}


	currentPhase->controller = &controller;
	currentPhase->menuIsShown = menu->isShown;

	// Phase timestep

	if ( ( ! currentPhase->isTerminated && ! menu->isShown ) || ! currentPhase->canPause ) {

		// Perform game actuators
		for ( int32_t i = 0, n = currentPhase->actuators.size(); i < n; i ++ ) {
			Actuator *actuator = currentPhase->actuators[ i ];
			actuator->actuate( dt, theTime );
		}

		for ( int32_t i = 0, n = currentPhase->actuatorsToBeDeleted.size(); i < n; i ++ ) {
			Actuator *actuator = currentPhase->actuatorsToBeDeleted[ i ];
			for ( uint32_t i = 0, n = currentPhase->actuators.size(); i < n; i ++ ) {
				if ( currentPhase->actuators[ i ] == actuator ) {
					actuator->finish( dt, theTime );
					delete actuator;
					currentPhase->actuators.erase( currentPhase->actuators.begin() + i );
					break;
				}
			}
		}
		currentPhase->actuatorsToBeDeleted.clear();

		for ( int32_t i = 0, n = currentPhase->actuatorsToBeAdded.size(); i < n; i ++ ) currentPhase->actuators.push_back( currentPhase->actuatorsToBeAdded[ i ] );
		currentPhase->actuatorsToBeAdded.clear();

		currentPhase->timestep( dt, theTime );
		theTime += dt;
	}

	// Menu timestep

	if ( menu->isShown ) {

		menu->timestep( dt, elapsedRealTime );

		if ( menu->result >= 0 ) {

			switch ( menu->result ) {
				case 0:
					// Start the game
					selectedPhaseIndex = 3;
					break;
				case 1:
					// Show instructions
					selectedPhaseIndex = 1;
					break;
				case 2:
					// Show about
					selectedPhaseIndex = 2;
					break;
				case 3:
					// Confirmed abort game
					selectedPhaseIndex = 0;
					break;
				case 4:
					// Confirmed exit game
					return SIM_TERMINATE;
				case 5:
					// Toggle music
					if ( sound->playingMusic ) {
						sound->stopMusic();
						sound->musicEnabled = false;
					} else {
						sound->musicEnabled = true;
						sound->playMusic( STARTING_TITLE_MUSIC_PATH );
					}

					break;
			}
			menu->hide();
		}

	} else if ( currentPhaseIndex == 0 && controller.a && theTime > timeLastKeypress + 0.3 ) selectedPhaseIndex = 3;

	// Phase logic
	if ( ( currentPhase->isTerminated && elapsedRealTime > 3.0 ) || selectedPhaseIndex >= 0 ) {

		// Current phase has ended. Determine next phase index

		int32_t previousPhaseIndex = currentPhaseIndex;

		if ( selectedPhaseIndex >= 0 ) currentPhaseIndex = selectedPhaseIndex;
		else if ( currentPhaseIndex == 0 ) currentPhaseIndex = 3;
		else if ( currentPhase->isSucceeded ) currentPhaseIndex ++;
		else if ( currentPhaseIndex == 1 || currentPhaseIndex == 2 || currentPhaseIndex == 7 ) currentPhaseIndex = 0;

		if ( currentPhaseIndex >= getNumPhases() ) currentPhaseIndex = 0;

		selectedPhaseIndex = -1;

		// Get starship status to pass it to the next phase
		StarshipData parameters = currentPhase->spaceshipActuator ? currentPhase->spaceshipActuator->getStarshipParameters() : StarshipData();

		// Recharge shield and energy to max
		parameters.shield = parameters.maxShield;
		parameters.energy = parameters.maxEnergy;

		// Store Hi-sccore
		if ( parameters.hiScore < parameters.score ) parameters.hiScore = parameters.score;

		// Terminate current phase
		currentPhase->terminatePhase();

		println( "delete currentPhase; ..." );
//		delete currentPhase;
		currentPhase = NULL;
		println( "delete currentPhase; OK" );

		// Create the new phase
		print( "createPhase; ..." );
		println( std::to_string(  currentPhaseIndex ) );
		currentPhase = createPhase( currentPhaseIndex );
		println( "createPhase; OK" );

		vid_border_color( 0, 0, 0 );

		theTime = 0;

		// Init new phase
		std::string error;
		if ( ! currentPhase->initPhase( error ) ) {
			println( std::string( "Error initing phase with index " ) + std::to_string( currentPhaseIndex ) + std::string( " : " ) + error );
			return SIM_TERMINATE;
		}

		// Restore starship status
		if ( currentPhase->spaceshipActuator ) {

			if ( currentPhaseIndex == 3 ) parameters.setDefaultValues();

			currentPhase->spaceshipActuator->setStarshipParameters( &parameters );

			if ( currentPhase->scoreActuator ) currentPhase->scoreActuator->screenScore = parameters.score;

		}

		// Start music
		if ( currentPhaseIndex == 0 && ! sound->playingMusic && previousPhaseIndex != 7 ) {
			sound->playMusic( STARTING_TITLE_MUSIC_PATH );
		} else if ( currentPhaseIndex == 7 ) {
			sound->playMusic( ENDING_TITLE_MUSIC_PATH );
		}// else if ( currentPhaseIndex > 2 ) sound->stopMusic();

	}

	// Draw scene

	glClearColor(
		currentPhase->backgroundColor.r,
		currentPhase->backgroundColor.g,
		currentPhase->backgroundColor.b,
		0.0
	);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if ( screenSaver ) drawScreenSaver( dt );
	else {

		pipeline.render( currentPhase->scene, currentPhase->camera );

		if ( menu->isShown ) {

			glDisable( GL_DEPTH_TEST );

			menu->render( menu->camera );

			glEnable( GL_DEPTH_TEST );

		}

	}

	// End of frame
	return SIM_CONTINUE;

}

/*
(Not used)
void SpaceGameSimulation::newViewPortSize( int width, int height ) {

	currentPhase->camera->aspect = ( ( float) width ) / ( ( float) height );
	currentPhase->camera->updateProjectionMatrix();

}
*/

bool SpaceGameSimulation::finish( std::string &error ) {

	//for ( int32_t i = 0, il = currentPhase->actuators.size(); i < il; i ++ ) delete currentPhase->actuators[ i ];

	// TODO

	currentPhase->terminatePhase();
	delete currentPhase;

	return pipeline.finish( error );

}

int32_t SpaceGameSimulation::getNumPhases() {
	return 8;
}

SpaceGamePhase *SpaceGameSimulation::createPhase( int32_t index ) {

	print( "createPhase: index: " );
	println( std::to_string( index ) );

	SpaceGamePhase *phase = NULL;
	switch ( index ) {
		case 0:
			phase = new StartingPhase();
			break;
		case 1:
			phase = new InstructionsPhase();
			break;
		case 2:
			phase = new AboutPhase();
			break;
		case 3:
			phase = new Phase1();
			break;
		case 4:
			phase = new Phase2();
			break;
		case 5:
			phase = new Phase4();
			break;
		case 6:
			phase = new Phase3();
			break;
		case 7:
			phase = new EndingPhase();
			break;
		default:
			phase = NULL;
			break;
	}

	if ( phase ) {
		phase->sound = sound;
		phase->font1 = font1;
	} else {

		print( "createPhase: Invalid phase index: " );
		println( std::to_string( index ) );

	}

	return phase;
}

bool SpaceGameSimulation::loadAudios( std::string &error ) {

	if ( ! sound->loadAudio( std::string( "explosion" ), error ) ) return false;

	if ( ! sound->loadAudio( std::string( "bigExplosion" ), error ) ) {
		error = "Could not load 'bigExplosion.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "collision" ), error ) ) {
		error = "Could not load 'collision.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "laser" ), error ) ) {
		error = "Could not load 'laser.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "enemyShot" ), error ) ) {
		error = "Could not load 'enemyShot.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "powerup" ), error ) ) {
		error = "Could not load 'powerup.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "menu1" ), error ) ) {
		error = "Could not load 'menu1.wav'";
		return false;
	}
	if ( ! sound->loadAudio( std::string( "menu2" ), error ) ) {
		error = "Could not load 'menu2.wav'";
		return false;
	}

	return true;

}

FPFont *SpaceGameSimulation::createFont1( std::string  &error ) {

	GL1Texture *charsetTexture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "fonts/font1/font1.png" ), error );
	if ( charsetTexture == NULL ) return NULL;

	FPFont *font = new FPFont();
	font->initWesternCharset();

	std::vector<Vector2> characterSizes;

	// space, A-S
	characterSizes.push_back( Vector2( 32, 87 ) );
	characterSizes.push_back( Vector2( 53, 87 ) );
	characterSizes.push_back( Vector2( 55, 87 ) );
	characterSizes.push_back( Vector2( 46, 87 ) );
	characterSizes.push_back( Vector2( 53, 87 ) );
	characterSizes.push_back( Vector2( 44, 87 ) );
	characterSizes.push_back( Vector2( 42, 87 ) );
	characterSizes.push_back( Vector2( 54, 87 ) );
	characterSizes.push_back( Vector2( 51, 87 ) );
	characterSizes.push_back( Vector2( 22, 87 ) );
	characterSizes.push_back( Vector2( 55, 87 ) );
	characterSizes.push_back( Vector2( 55, 87 ) );
	characterSizes.push_back( Vector2( 44, 87 ) );
	characterSizes.push_back( Vector2( 63, 87 ) );
	characterSizes.push_back( Vector2( 55, 87 ) );
	characterSizes.push_back( Vector2( 52, 87 ) );
	characterSizes.push_back( Vector2( 52, 87 ) );
	characterSizes.push_back( Vector2( 53, 87 ) );
	characterSizes.push_back( Vector2( 56, 87 ) );
	characterSizes.push_back( Vector2( 45, 87 ) );

	// T-Z
	characterSizes.push_back( Vector2( 54, 87 ) );
	characterSizes.push_back( Vector2( 48, 87 ) );
	characterSizes.push_back( Vector2( 47, 87 ) );
	characterSizes.push_back( Vector2( 63, 87 ) );
	characterSizes.push_back( Vector2( 46, 87 ) );
	characterSizes.push_back( Vector2( 47, 87 ) );
	characterSizes.push_back( Vector2( 51, 87 ) );

	// 0-9
	characterSizes.push_back( Vector2( 59, 87 ) );
	characterSizes.push_back( Vector2( 31, 87 ) );
	characterSizes.push_back( Vector2( 37, 87 ) );
	characterSizes.push_back( Vector2( 45, 87 ) );
	characterSizes.push_back( Vector2( 53, 87 ) );
	characterSizes.push_back( Vector2( 45, 87 ) );
	characterSizes.push_back( Vector2( 53, 87 ) );
	characterSizes.push_back( Vector2( 46, 87 ) );
	characterSizes.push_back( Vector2( 59, 87 ) );
	characterSizes.push_back( Vector2( 52, 87 ) );

	// / ( ) . -
	characterSizes.push_back( Vector2( 47, 87 ) );
	characterSizes.push_back( Vector2( 28, 87 ) );
	characterSizes.push_back( Vector2( 28, 87 ) );
	characterSizes.push_back( Vector2( 22, 87 ) );
	characterSizes.push_back( Vector2( 33, 87 ) );

	// _ + * = : ! ,
	characterSizes.push_back( Vector2( 46, 87 ) );
	characterSizes.push_back( Vector2( 55, 87 ) );
	characterSizes.push_back( Vector2( 28, 87 ) );
	characterSizes.push_back( Vector2( 47, 87 ) );
	characterSizes.push_back( Vector2( 22, 87 ) );
	characterSizes.push_back( Vector2( 26, 87 ) );
	characterSizes.push_back( Vector2( 22, 90 ) );

	font->generateUVs( characterSizes, charsetTexture );

	return font;

}

void SpaceGameSimulation::drawScreenSaver( float dt ) {

	Vector3 increm;
	increm.copy( &screenSaverDirection );
	increm.multiplyScalar( dt * 2 );
	screenSaverPos.x += increm.x;
	screenSaverPos.y += increm.y;


	float lx = 4;
	float ly = 4;

	if ( screenSaverPos.x < - lx - 4) {
		screenSaverDirection.x = 1;
		screenSaverPos.x = - lx - 4;
	}

	if ( screenSaverPos.y < - ly ) {
		screenSaverDirection.y = 1;
		screenSaverPos.y = - ly;
	}

	if ( screenSaverPos.x > lx ) {
		screenSaverDirection.x = -1;
		screenSaverPos.x = lx;
	}
	if ( screenSaverPos.y > ly ) {
		screenSaverDirection.y = -1;
		screenSaverPos.y = ly;
	}

	screenSaverText->pose->position.copy( &screenSaverPos );
	screenSaverText->pose->updateMatrix();

	pipeline.render( screenSaverScene, screenSaverCamera );

}
