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

#include "StartingPhase.h"

#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

using namespace ceres;

StartingPhase::StartingPhase() : SpaceGamePhase() {
}

StartingPhase::~StartingPhase() {
}

bool StartingPhase::initPhase( std::string &error ) {

	canPause = false;

	doStartGame = false;
	timeDoStartGame = 0;

	numWaves = 0;
	allowCreatePowerup = false;

	//backgroundColor.set( 118.0 / 255.0, 148.0 / 255.0, 184.0 / 255.0, 0.0 );
	backgroundColor.set( 0.0, 0.0, 0.0, 0.0 );

	// Camera

	camera = new Camera();
	camera->pose->position.set( 10, 70, 0 );
	camera->pose->updateMatrix();

	// Scene

	scene = new GL1Scene();
	//scene->init();

	// Floor

	drawFloor = true;
	floorGrid = new Grid();
	floorGrid->pose = new Pose();
	zViewDistance = 450.0;
	floorGrid->zViewDistance = zViewDistance;
	squareLineWidth = 3.0;
	floorGrid->squareLineWidth = squareLineWidth;
	scene->objects.push_back( floorGrid );

	GL1ObjectUtils objectUtils;

	// Lighting

	setDefaultLight( scene );

	// Title object

	GL1Material *titleMaterial = new GL1Material();
	titleMaterial->diffuse.set( 1, 1, 1 );
	titleMaterial->texture = font1->texture;
	titleMaterial->transparent = true;
	titleMaterial->opacity = 1.0;

	titleObject = new GL1Text3D();
	titleObject->pose = new Pose();
	titleObject->pose->position.set( - 7.77, - 1.5, 0 )->inc( &camera->pose->position );
	titleObject->pose->updateMatrix();
	titleObject->init( font1, titleMaterial, Vector3( 1.75, 1.75, 1 ) );
	titleObject->setString( "Starship Madness" );
	scene->objects.push_back( titleObject );

	Vector3 axis1( 1, 0, 0 );
	Vector3 axis2( 1, 0, 0 );
	titleQ0.setFromAxisAngle( &axis1, 0 );
	titleQ1.setFromAxisAngle( &axis2, - M_PI * 0.5 );

	// Main authors text

	GL1Material *textMaterial = new GL1Material();
	textMaterial->illuminated = false;
	textMaterial->diffuse.set( 1, 1, 1 );
	textMaterial->texture = font1->texture;
	textMaterial->transparent = true;
	textMaterial->opacity = 1.0;

	text = new GL1Text3D();
	text->pose = new Pose();
	text->pose->position.set( 6, camera->pose->position.y - 2, - 8 );
	text->pose->updateMatrix();
	text->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text->setString( "Concept, Code: yomboprime\nMusic: airsynth\nIdeas, motivation: Carles\n2024" );
	scene->objects.push_back( text );

	// Press start text

	text2 = new GL1Text3D();
	text2->pose = new Pose();
	text2->pose->position.set( 8.4, camera->pose->position.y - 4, - 8 );
	text2->pose->updateMatrix();
	text2->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
	text2->setString( "Press START" );
	scene->objects.push_back( text2 );

	// Create spaceship
	spaceshipActuator = createSpaceship( false, error );
	if ( spaceshipActuator == NULL ) return false;

	hiScoreText = NULL;

	scoreActuator = NULL;

	return true;

}

void StartingPhase::terminatePhase() {
	delete camera;
	delete scene;
}

void StartingPhase::timestep( float dt, float time ) {

	// Create Hi-score text

	if ( hiScoreText == NULL ) {

		GL1Material *textMaterial = new GL1Material();
		textMaterial->illuminated = false;
		textMaterial->diffuse.set( 1, 1, 1 );
		textMaterial->texture = font1->texture;
		textMaterial->transparent = true;
		textMaterial->opacity = 1.0;

		hiScoreText = new GL1Text3D();
		hiScoreText->pose = new Pose();
		hiScoreText->pose->position.set( 8.5, camera->pose->position.y + 4, - 8 );
		hiScoreText->pose->updateMatrix();
		hiScoreText->init( font1, textMaterial, Vector3( 0.5, 0.5, 1 ) );
		hiScoreText->setString( "Hi-score: " + std::to_string( spaceshipActuator->parameters.hiScore ) );
		hiScoreText->visible = spaceshipActuator->parameters.hiScore > 0;
		scene->objects.push_back( hiScoreText );

	}

	// Move the grid

	camera->pose->position.z -= dt * 80.0;
	camera->pose->updateMatrix();

	floorGrid->pose->position.z = camera->pose->position.z;
	floorGrid->pose->updateMatrix();

	text->pose->position.z = camera->pose->position.z - 8;
	text->pose->updateMatrix();
	text->visible = time > 3 && ! menuIsShown;

	text2->pose->position.z = camera->pose->position.z - 8;
	text2->pose->updateMatrix();
	text2->visible = time > 3 && ( ( (int32_t)( time - 3 ) ) & 2 ) && ! menuIsShown;

	hiScoreText->pose->position.z = camera->pose->position.z - 8;
	hiScoreText->pose->updateMatrix();
	hiScoreText->visible = ! menuIsShown;

	if ( ! doStartGame && terminationRequested && time > 2.0 ) {
		doStartGame = true;
		timeDoStartGame = time;
	}

	if ( doStartGame ) {

		if ( time > timeDoStartGame + 1.5 ) isTerminated = true;
		else titleObject->pose->position.z = camera->pose->position.z - 10 + ( time - timeDoStartGame ) * 12.0 / 2.5;

	} else {

		float f = 1.0 - minim( 1.0, time / 4.0 );
		titleObject->pose->position.z = camera->pose->position.z - 10 + f * 12;
		titleObject->pose->rotation.interpolateQuaternions( &titleQ0, &titleQ1, f );
		titleObject->pose->updateMatrix();
		titleObject->visible = ! menuIsShown;

	}

}
