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

#include "ScoreActuator.h"
#include "../SpaceGamePhase.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Text3DScreen.h"

#include "console/console.h"

using namespace ceres;

ScoreActuator::ScoreActuator() {
}

ScoreActuator::~ScoreActuator(){
}

bool ScoreActuator::init( float dt, float time, std::string &error ) {

	object = NULL;

	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;

	GL1Material *material1 = new GL1Material();
	material1->diffuse.set( 0.5, 0.7, 0.5 );
	material1->specular.set( 0, 0, 0 );
	material1->illuminated = false;
	material1->texture = phase1->font1->texture;
	material1->transparent = true;
	material1->opacity = 1.0;

	scoreText = new GL1Text3DScreen();
	scoreText->pose = new Pose();
	scoreText->pose->position.set( -7.3, 4.2, - 10 );
	scoreText->pose->updateMatrix();
	scoreText->init( phase1->font1, material1, Vector3( 1.2, 1.2, 1 ) );
	scoreText->setString( "0" );
	phase1->scene->objects.push_back( scoreText );

	((Phase *)game)->actuatorsToBeAdded.push_back( this );

	screenScore = 0;

	return true;

}

void ScoreActuator::actuate( float dt, float time ) {

	if ( spaceshipActuator->parameters.score > screenScore ) screenScore += 10;
	else if ( spaceshipActuator->parameters.score < screenScore ) screenScore -= 10;

	scoreText->setString( std::to_string( screenScore ) );

}


void ScoreActuator::finish( float dt, float time ) {

	// not needed ((Phase *)phase)->actuatorsToBeDeleted.push_back( this );

}
