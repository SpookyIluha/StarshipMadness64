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

#include "AboutPhase.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "console/console.h"

#include "utils/utils.h"

#include "arch/arch.h"

using namespace ceres;

AboutPhase::AboutPhase() : SpaceGamePhase() {
}

AboutPhase::~AboutPhase() {
}

bool AboutPhase::initPhase( std::string &error ) {

	canPause = false;

	numWaves = 0;
	nextTimeCreatePowerup = 0.0;
	allowCreatePowerup = false;
	drawFloor = false;
	spaceshipActuator = NULL;
	scoreActuator = NULL;

	scene = NULL;

	backgroundColor.set( 0, 0, 0.15, 0);

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

	// Text

	GL1Material *textMaterial = new GL1Material();
	textMaterial->illuminated = false;
	textMaterial->diffuse.set( 1, 1, 1 );
	textMaterial->texture = font1->texture;
	textMaterial->transparent = true;
	textMaterial->opacity = 1.0;

	GL1Text3D* text = new GL1Text3D();
	text->pose = new Pose();
	text->pose->position.set( -6, 3.5, - 8 );
	text->pose->updateMatrix();
	text->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text->setString( "Starship Madness\n\nMade with KOS for the DreamDisk 2024\n\nConcept, Code: Yomboprime\nMusic: Airsynth\nHardware ideas testing motivation: Carles\n\nLicense: GPL2.0, KOS License\n\nShip model: Gundy\nSound effects: phoenix1291\nFont: Andrew Young (Creative commons 4.0)\n\n\n                                                   1/3" );
	scene0->objects.push_back( text );

	// *************************************************************
	// Scene 1
	// *************************************************************

	GL1Scene *scene1 = new GL1Scene();
	//scene1->init();
	setDefaultLight( scene1 );

	// Text

	licenseText = new GL1Text3D();
	licenseText->pose = new Pose();
	licenseText->pose->position.set( - 6.5, 3.5, - 8 );
	licenseText->pose->updateMatrix();
	licenseText->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	std::string theString =
		std::string( "Homebrew game made with KallistiOS\n\n" ) +
		std::string( "License GPL2 and KOS License\n\n" ) +
		std::string( kos_get_authors() ) + std::string( "\n\n" ) +
		std::string( kos_get_license() ) + std::string( "\n\n" ) +
		std::string( "Code and documentation available at\nhttps://codeberg.org/yombo/smadness\n\n\n" ) +
		std::string( "                                                   2/3" );
	licenseText->setString( stringLimitLength( theString, 48 ) );
	scene1->objects.push_back( licenseText );

	// *************************************************************
	// Scene 2
	// *************************************************************

	GL1Scene *scene2 = new GL1Scene();
	//scene0->init();
	setDefaultLight( scene2 );

	// Text

	GL1Text3D *text2 = new GL1Text3D();
	text2->pose = new Pose();
	text2->pose->position.set( -6, 3.5, - 8 );
	text2->pose->updateMatrix();
	text2->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text2->setString( "\n\n\nDedicated to our families with love.\n\n\n\n\n\n\n\n\n\n\n\n\n                                                   3/3" );
	scene2->objects.push_back( text2 );

	// Initial scene

	scenes[ 0 ] = scene0;
	scenes[ 1 ] = scene1;
	scenes[ 2 ] = scene2;

	scene = scene0;
	currentSceneIndex = 0;

	prevPressedTime = 0;
	time0 = 0;

	return true;

}

void AboutPhase::terminatePhase() {

	delete camera;
	for ( uint32 i = 0; i < NUM_SCENES; i ++ ) delete scenes[ i ];

//	delete collisionables[ "starship" ];

}

void AboutPhase::timestep( float dt, float time ) {

	if ( controller->a ) {
		if ( time > prevPressedTime + 0.3 ) {
			currentSceneIndex ++;
			sound->playAudio( "menu1" );
			if ( currentSceneIndex >= NUM_SCENES ) isTerminated = true;
			else {
				scene = scenes[ currentSceneIndex ];
				time0 = time;
			}
		}
		prevPressedTime = time;
	}

	Vector3 axis( 0, 1, 0 );

	switch ( currentSceneIndex ) {
		case 0:
			break;
		case 1:
			if ( time - time0 > 4.5 && licenseText->pose->position.y < 43 ) {
				licenseText->pose->position.y += dt * 0.8;
				licenseText->pose->updateMatrix();
			}
			break;
		case 2:
			break;
		default:
			break;
	}

}
