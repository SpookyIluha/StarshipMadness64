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

//#include <dc/video.h>
#include <libdragon.h>
#include <GL/gl_integration.h>

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

#define STARTING_TITLE_MUSIC_PATH ( "rom:/audios/lasflores.wav64" )
#define ENDING_TITLE_MUSIC_PATH ( "rom:/audios/elviciodelsilicio.wav64" )

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

	//mapleController = maple_enum_type( 0, MAPLE_FUNC_CONTROLLER );

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
	screenSaverScene = new GL1Scene();
	screenSaver = false;
	timeLastKeypress = 0;
	screenSaverDirection.set( 1, - 1, 0 );
	screenSaverPos.set( 0.54321, 0, - 8 );

	GL1Material *textMaterial = new GL1Material();
	textMaterial->illuminated = false;
	textMaterial->diffuse.set( 1, 1, 1 );
	textMaterial->texture = font1->texture;
	textMaterial->transparent = true;
	textMaterial->opacity = 0.0;
	textMaterial->depthTest = false;

	screenSaverText = new GL1Text3D();
	screenSaverText->pose = new Pose();
	screenSaverText->init( font1, textMaterial, Vector3( 1, 1, 1 ) );
	screenSaverText->setString( "Starship madness" );
	screenSaverScene->objects.push_back( screenSaverText );

	// Start music
	if ( ! sound->playMusic( STARTING_TITLE_MUSIC_PATH ) ) {
		debugf( "Could not start the music" );
	}

	return true;

}

Simulation::SimulationStepResult SpaceGameSimulation::timestep( scalar dt, scalar elapsedRealTime ) {

	// Read controller
	joypad_poll();
	joypad_inputs_t state = joypad_get_inputs(JOYPAD_PORT_1);

	mixer_try_play();

	controller.x1 = state.stick_x / 128.0;
	controller.y1 = state.stick_y / 128.0;
	controller.x2 = state.stick_x / 128.0;
	controller.y2 = state.stick_y / 128.0;
	controller.a = state.btn.a;
	controller.b = state.btn.b;
	controller.c = state.btn.c_left;
	controller.d = state.btn.c_down;
	controller.up = state.btn.d_up || state.btn.c_up;
	controller.down = state.btn.d_down || state.btn.c_down;
	controller.left = state.btn.d_left || state.btn.c_left;
	controller.right = state.btn.d_right || state.btn.c_right;
	controller.leftTrigger = state.analog_l / 255.0;
	controller.rightTrigger = state.analog_r / 255.0;
	controller.start = state.btn.start;
	controller.z = state.btn.z;
	controller.l = state.btn.l || state.btn.r;
	//controller.r = state.btn.r;

	// Quick exit from program
	if ( controller.start && controller.a ) {
		 return SIM_TERMINATE;
	}

	// Screen saver
	bool btnpress = ( state.stick_x > 30 || state.stick_y > 30 || state.btn.b || state.btn.d_up || state.btn.d_down || state.btn.c_up || state.btn.c_down || state.btn.c_left || state.btn.d_left || state.btn.c_right || state.btn.d_right || state.btn.z || state.btn.l || state.btn.r || state.btn.start);
	if ( screenSaver ) {
		if ( ( btnpress) ) {
			screenSaver = false;
			timeLastKeypress = elapsedRealTime;
		}
	} else {
		if ( ! ( btnpress)) {
			if ( elapsedRealTime > timeLastKeypress + 300 ) screenSaver = true;
		} else timeLastKeypress = elapsedRealTime;
	}

	// Hide / show menu
	if ( controller.start && elapsedRealTime > menu->nextTimePressed ) {

		menu->nextTimePressed = elapsedRealTime + 0.3;
		if ( menu->isShown ) menu->hide();
		else menu->show( currentPhaseIndex > 2 );
	}

	mixer_try_play();
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
		mixer_try_play();
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

		debugf( "delete currentPhase; ..." );
		delete currentPhase;
		currentPhase = NULL;
		debugf( "delete currentPhase; OK" );

		// Create the new phase
		debugf( "createPhase; ..." );
		debugf( "%ld", currentPhaseIndex );
		currentPhase = createPhase( currentPhaseIndex );
		debugf( "createPhase; OK" );

		theTime = 0;

		// Init new phase
		std::string error;
		if ( ! currentPhase->initPhase( error ) ) {
			debugf( "Error initing phase with index %ld : %s" , currentPhaseIndex, error.c_str() );
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
	mixer_try_play();
	rdpq_attach(display_get(), display_get_zbuf());
	gl_context_begin();

	glClearColor(
		currentPhase->backgroundColor.r,
		currentPhase->backgroundColor.g,
		currentPhase->backgroundColor.b,
		0.0
	);
	/*glClearColor(
		0.2,
		0.2,
		0.2,
		1.0
	);*/
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable( GL_DEPTH_TEST );

	glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(10.0f,10.0f,10.0f);
	glPushMatrix();

	if ( screenSaver ) drawScreenSaver( dt );
	else {

		pipeline.render( currentPhase->scene, currentPhase->camera );

		if ( menu->isShown ) {

			glEnable( GL_DEPTH_TEST );

			menu->render( menu->camera );

			glEnable( GL_DEPTH_TEST );

		}

	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopMatrix();

	gl_context_end();

	heap_stats_t stats;
	sys_get_heap_stats(&stats);

	//rdpq_text_printf(NULL, 1, 40,40, "FPS: %.2f", display_get_fps());
	//rdpq_text_printf(NULL, 1, 40,50, "Mem: %i B", stats.used);
	//rdpq_text_printf(NULL, 1, 40,60, "Time: %.2f ", elapsedRealTime);


	rdpq_detach_show();
	mixer_try_play();
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

	debugf( "createPhase: index: " );
	debugf( "%ld", index );

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

		debugf( "createPhase: Invalid phase index: " );
		debugf( "%ld", index );

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

	GL1Texture *charsetTexture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "textures/font2.ia4.sprite" ), error );
	if ( charsetTexture == NULL ) return NULL;

	FPFont *font = new FPFont();
	font->initWesternCharset();

	font->RDPQfontLarge = rdpq_font_load((SPACESHIP_MADNESS_DIR + std::string("fonts/LazenbyCompLiquid_Large.font64")).c_str());
	font->RDPQfontSmall = rdpq_font_load((SPACESHIP_MADNESS_DIR + std::string("fonts/LazenbyCompLiquid_Small.font64")).c_str());
	rdpq_text_register_font(2, font->RDPQfontLarge);
	rdpq_text_register_font(3, font->RDPQfontSmall);

	rdpq_fontstyle_t style1;
	style1.color = RGBA32(255,255,255,255);
	style1.outline_color = RGBA32(0,0,0,255);
	rdpq_font_style(font->RDPQfontLarge, 0, &style1);
	rdpq_font_style(font->RDPQfontSmall, 0, &style1);
	rdpq_fontstyle_t style2;
	style2.color = RGBA32(25,64,178,255);
	style2.outline_color = RGBA32(0,0,0,255);
	rdpq_font_style(font->RDPQfontLarge, 1, &style2);
	rdpq_font_style(font->RDPQfontSmall, 1, &style2);
	rdpq_fontstyle_t style3;
	style3.color = RGBA32(127,178,127,255);
	style3.outline_color = RGBA32(0,0,0,255);
	rdpq_font_style(font->RDPQfontLarge, 2, &style3);
	rdpq_font_style(font->RDPQfontSmall, 2, &style3);

	std::vector<Vector2> characterSizes;

	// space, A-S
	characterSizes.push_back( Vector2( 32.0 / 8.0, 87.0 / 8.0 ) );
	characterSizes.push_back( Vector2( 53.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 55.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 46.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 53.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 44.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 42.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 54.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 51.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 22.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 55.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 55.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 44.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 63.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 55.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 52.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 52.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 53.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 56.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 45.0 / 8.0, 87.0 / 8.0) );

	// T-Z
	characterSizes.push_back( Vector2( 54.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 48.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 47.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 63.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 46.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 47.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 51.0 / 8.0, 87.0 / 8.0) );

	// 0-9
	characterSizes.push_back( Vector2( 59.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 31.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 37.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 45.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 53.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 45.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 53.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 46.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 59.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 52.0 / 8.0, 87.0 / 8.0) );

	// / ( ) . -
	characterSizes.push_back( Vector2( 47.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 28.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 28.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 22.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 33.0 / 8.0, 87.0 / 8.0) );

	// _ + * = : ! ,
	characterSizes.push_back( Vector2( 46.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 55.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 28.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 47.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 22.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 26.0 / 8.0, 87.0 / 8.0) );
	characterSizes.push_back( Vector2( 22.0 / 8.0, 90.0 / 8.0 ) );

	font->generateUVs( characterSizes, charsetTexture );

	return font;

}

void SpaceGameSimulation::drawScreenSaver( float dt ) {

	Vector3 increm;
	increm.copy( &screenSaverDirection );
	increm.multiplyScalar( dt * 80 );
	screenSaverPos.x += increm.x;
	screenSaverPos.y += increm.y;

	//rdpq_text_printf(NULL, 1, 20,20 , "screenSaverPos: %.2f, %.2f", screenSaverPos.x, screenSaverPos.y);
	//rdpq_text_printf(NULL, 1, 20,30 , "dt: %.2f", dt);

	float lx = 640;
	float ly = 480;

	if ( screenSaverPos.x < 0) {
		screenSaverDirection.x = 1;
		screenSaverPos.x = 0;
	}

	if ( screenSaverPos.y < 0 ) {
		screenSaverDirection.y = 1;
		screenSaverPos.y = 0;
	}

	if ( screenSaverPos.x > lx - 200) {
		screenSaverDirection.x = -1;
		screenSaverPos.x = lx- 200 ;
	}
	if ( screenSaverPos.y > ly ) {
		screenSaverDirection.y = -1;
		screenSaverPos.y = ly;
	}

	screenSaverText->pose->position.copy( &screenSaverPos );
	screenSaverText->pose->updateMatrix();

	pipeline.render( screenSaverScene, screenSaverCamera );

}
