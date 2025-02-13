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

#include "SpaceGamePhase.h"

#include "math3d/Group.h"

#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Scene.h"

#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

#include "actuators/PowerupActuator.h"

#include "console/console.h"
#include "objects/GL1LightEye.h"

#include "SpaceshipMadnessConfig.h"

using namespace ceres;

SpaceGamePhase::SpaceGamePhase() :
	terminationRequested( false ),
	drawFloor( false ),
	drawCeiling( false ),
	ceilingHeight( 3300.0 ),
	floorColor( 0, 1, 0 ),
	ceilingColor( 1, 0, 0 ),
	squareLength( 40.0 ),
	squareLineWidth( 2.0 ),
	zViewDistance( 350 ),
	floorGrid( NULL ),
	ceilingGrid( NULL ),
	citySize( 1200.0 ),
	spaceshipActuator( NULL ),
	canPause( false ) {
}

SpaceGamePhase::~SpaceGamePhase() {
}

SpaceshipActuator *SpaceGamePhase::createSpaceship( bool addActuator, std::string &error ) {

	// Materials

	GL1Material *bodyMaterial = new GL1Material();
	bodyMaterial->diffuse.set( 0.8, 0.3, 0.05 );
	bodyMaterial->minZ = 4.0f;
	bodyMaterial->maxZ = 500.0f;
	bodyMaterial->depthTest = true;
	//bodyMaterial->specular.copy( &bodyMaterial->diffuse );
	//bodyMaterial->specularExponent = 0.01;

	GL1Material *bodyWhiteMaterial = new GL1Material();
	bodyWhiteMaterial->diffuse.set( 0.8, 0.8, 0.8 );
	bodyWhiteMaterial->minZ = 4.0f;
	bodyWhiteMaterial->maxZ = 500.0f;
	bodyWhiteMaterial->depthTest = true;
	//bodyWhiteMaterial->specular.copy( &bodyWhiteMaterial->diffuse );
	//bodyWhiteMaterial->specularExponent = 0.01;

	GL1Material *engineMaterial = new GL1Material();
	engineMaterial->diffuse.set( 0.2, 0.2, 0.2 );
	engineMaterial->minZ = 4.0f;
	engineMaterial->maxZ = 500.0f;
	engineMaterial->depthTest = true;
	//engineMaterial->specular.copy( &engineMaterial->diffuse );
	//engineMaterial->specularExponent = 0.01;

	GL1Material *glassMaterial = new GL1Material();
	glassMaterial->diffuse.set( 0.01, 0, 0.1 );
	glassMaterial->minZ = 4.0f;
	glassMaterial->maxZ = 500.0f;
	glassMaterial->depthTest = true;

	GL1Material *laserMaterial = new GL1Material();
	laserMaterial->illuminated = false;
	laserMaterial->diffuse.set( 0, 0.7, 1 );
	laserMaterial->minZ = 4.0f;
	laserMaterial->maxZ = 500.0f;
	laserMaterial->depthTest = true;
	//glassMaterial->specular.copy( &glassMaterial->diffuse );
	//glassMaterial->specularExponent = 0.01;

	// Create the drawable object

	GL1ObjectUtils objectUtils;
	Vector3 pos;

	Group* spaceshipObject = new Group();
	Pose* p = new Pose();


	//assert(0);
	GL1Mesh *body = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/bodywhite.stl" ), bodyWhiteMaterial, 10.0, pos, error, new GL1Mesh(), 1.0, true );
	GL1Mesh *bodyOrange = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/bodyorange.stl" ), bodyMaterial, 10.0, pos, error, new GL1Mesh(), 1.0, true );
	GL1Mesh *engine = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/engine.stl" ), engineMaterial, 10.0, pos, error );
	GL1Mesh *glass = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/glass.stl" ), glassMaterial, 10.0, pos, error, new GL1Mesh(), 1.0, true );
	GL1Mesh *laser = objectUtils.createObject( SPACESHIP_MADNESS_DIR + std::string( "stls/bodylaser.stl" ), laserMaterial, 10.0, pos, error, new GL1Mesh(), 1.0, true );

	if ( ! body ) {
		error = "Could not load bodywhite.stl";
		return NULL;
	}
	if ( ! bodyOrange ) {
		error = "Could not load bodyorange.stl";
		return NULL;
	}
	if ( ! engine ) {
		error = "Could not load engine.stl";
		return NULL;
	}
	if ( ! glass ) {
		error = "Could not load glass.stl";
		return NULL;
	}

	spaceshipObject->objects.push_back( body );
	spaceshipObject->objects.push_back( bodyOrange );
	spaceshipObject->objects.push_back( engine );
	spaceshipObject->objects.push_back( glass );
	spaceshipObject->objects.push_back( laser );

	// Create spaceship actuator
	SpaceshipActuator *spaceshipActuator = new SpaceshipActuator();
	spaceshipActuator->spaceshipObject = spaceshipObject;
	spaceshipActuator->camera = camera;
	spaceshipActuator->game = addActuator ? this : NULL;
	spaceshipActuator->laserobj = laser;
	if ( ! spaceshipActuator->init( 0.0, 0.0, error ) ) return NULL;
	if ( addActuator ) actuators.push_back( spaceshipActuator );

	spaceshipObject->pose = p;

	//spaceshipObject->objects.push_back( spaceshipObject->objects[4] );

	// Create starship collisionable
	RadiusCollisionable *starshipCol = new RadiusCollisionable();
	starshipCol->position = &spaceshipActuator->spaceshipObject->pose->position;
	starshipCol->radius = spaceshipActuator->spaceshipObject->radius;
	collisionables[ "starship" ] = starshipCol;

	spaceshipActuator->object = spaceshipObject;

	return spaceshipActuator;

}

PowerupActuator *SpaceGamePhase::createPowerup( int32_t powerupType, float dt, float time, std::string &error ) {

	if ( powerupType < 0 ) {

		float r = frand();
		if ( r < 0.4 ) powerupType = PowerupActuator::POWERUP_REG_ENERGY;
		else if ( r < 0.6 ) powerupType = PowerupActuator::POWERUP_MAX_ENERGY;
		else if ( r < 0.8 ) powerupType = PowerupActuator::POWERUP_MAX_SHIELD;
		else powerupType = PowerupActuator::POWERUP_REG_SHIELD;

	} if ( powerupType > 3 ) powerupType = 0;

	PowerupActuator *powUp = new PowerupActuator();
	powUp->game = this;
	powUp->spaceshipActuator = spaceshipActuator;

	powUp->powerupType = powerupType;
	if ( ! powUp->init( dt, time, error ) ) {
		delete powUp;
		return NULL;
	}

	powerups.push_back( powUp );

	return powUp;
}

bool SpaceGamePhase::placeObject( Posable *object, Collisionable *collisionable, float extentsRange, float minHeight, float maxHeight, Vector3 offset ) {

	// Optional collisionable obstaculizes object placement

	Vector3 pos;
	bool placed = false;

	for ( int32_t attempts = 0; attempts < 20; attempts ++ ) {

		pos.set( frand() - 0.5, frand() - 0.5, 0 );
		pos.multiplyScalar( extentsRange );
		pos.y = minHeight + ( maxHeight - minHeight ) * frand();

		if ( ! collisionable || ! collisionable->isPointInside( &pos ) ) {
			placed = true;
			break;
		}
	}

	if ( ! placed ) return false;

	object->pose->position.copy( &pos );
	object->pose->updateMatrix();

	return true;

}

void SpaceGamePhase::setDefaultLight( Scene *scene ) {

	GL1Scene *s = (GL1Scene *)scene;

	s->lights.clear();
	GL1LightEye *light0 = new GL1LightEye();
	s->lights.push_back( light0 );
	light0->lightIndex = 0;
	light0->enabled = true;
	light0->isSpot = false;
	light0->position.set( 0.0, 0.0, 1.0 );
	light0->ambient.set( 0, 0, 0 );
	light0->diffuse.set( 1, 1, 1 );
	//light0->specular.set( 0, 0, 0 );
	light0->updateParams();

}

void SpaceGamePhase::explodeMeshRandomly( Mesh *mesh, float randomization, float dt ) {
	if(mesh == NULL) return;
	// No attribute check, assumed only position and normal
	if(mesh->block){
		rspq_wait();
		rspq_block_free(mesh->block);
		mesh->block = NULL;
	}

	int32_t stride = 2 * 3;
	int32_t normalOffset = 3;
	float *p = mesh->vertices;
	Vector3 pos;
	Vector3 normal;
	Vector3 increm;
	for ( int32_t i = 0, n = mesh->numVertices; i < n; i ++ ) {
		pos.copyFromArray( p );
		normal.copyFromArray( p, normalOffset );

		increm.set( frand() * 2 - 1, frand() * 2 - 1, frand() * 2 - 1 );
		increm.multiplyScalar( randomization );
		normal.inc( &increm )->normalize();

		normal.copyToArray( p, normalOffset );

		normal.multiplyScalar( 25.0 * dt );
		pos.inc( &normal );

		pos.copyToArray( p );

		p += stride;
	}

}
