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

#include "EndingPhase.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"
#include "fixedPipeline/gl1/GL1Texture.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "math3d/importexport/MeshImporter.h"

#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

#include "../SpaceshipMadnessConfig.h"

using namespace ceres;

EndingPhase::EndingPhase() : SpaceGamePhase() {
}

EndingPhase::~EndingPhase() {
}

bool EndingPhase::initPhase( std::string &error ) {

	numWaves = 0;

	backgroundColor.set( 0, 0, 0, 0 );

	// Camera

	camera = new Camera();
	camera->pose->position.set( 0, 0, 0 );
	camera->pose->updateMatrix();

	// Scene

	scene = new GL1Scene();
	//scene->init();

	GL1ObjectUtils objectUtils;

	// Lighting

	setDefaultLight( scene );

	GL1Material *bandMaterial = new GL1Material();
	bandMaterial->illuminated = false;
	bandMaterial->diffuse.set( 1, 1, 1 );
	bandMaterial->specular.set( 0, 0, 0 );
	bandMaterial->doubleSided = true;
	bandMaterial->depthTest = false;
	GL1Texture *bandTexture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "textures/gradient1.png" ), error );
	if ( bandTexture == NULL ) return NULL;
	bandMaterial->texture = bandTexture;

	Vector3 bandPosition( 0, 0, - 180 );
	band = objectUtils.createPNCObject(
		SPACESHIP_MADNESS_DIR + std::string( "models/charset/congrats.xyzuv" ),
		bandMaterial,
		10,
		bandPosition,
		error,
		new GL1Mesh(),
		1,
		false
	);
	if ( ! band ) return false;
	band->mesh->center();
	scene->objects.push_back( band );

	Vector3 axis1( 1, 0, 0 );
	bandQ0.setFromAxisAngle( &axis1, 0 );
	bandQ1.setFromAxisAngle( &axis1, - M_PI * 0.5 );

	scoreActuator = NULL;

	dismissed = false;

	return true;

}

void EndingPhase::timestep( float dt, float time ) {

	if ( dismissed && time > timeDismissed + 20 ) isTerminated = true;

	if ( time > 5 && controller->a && ! dismissed ) {

		dismissed = true;
		timeDismissed = time;

	}

	// Color cycling

	float *verts = band->mesh->vertices;
	for ( int32_t i = 0, n = band->mesh->numVertices; i < n; i ++ ) {

		// Change U coordinate
		float u = verts[ i * 5 + 3 ];
		u += 0.8 * dt;
		if ( u >= 1 ) u --;
		verts[ i * 5 + 3 ] = u;

	}

	// Movement

	if ( ! dismissed ) band->pose->position.z = maxim( - 45 * time, - 180 );
	else band->pose->position.z = - 180 - maxim( 0, ( time - timeDismissed ) ) * 60;

	// Rotation

	float f = 1 - minim( 1.0, time / 4.0 );
	band->pose->rotation.interpolateQuaternions( &bandQ0, &bandQ1, f );
	band->pose->updateMatrix();


}

void EndingPhase::terminatePhase() {

	delete camera;
	delete scene;

}
