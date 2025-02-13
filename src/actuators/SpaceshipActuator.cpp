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

#include "SpaceshipActuator.h"
#include "../SpaceGamePhase.h"
#include "math3d/Group.h"
#include "fixedPipeline/gl1/GL1Parallelepiped.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"

#include "console/console.h"

#include "../SpaceshipMadnessConfig.h"

#include "time/time.h"

using namespace ceres;

SpaceshipActuator::SpaceshipActuator() {
}

SpaceshipActuator::~SpaceshipActuator(){
}

bool SpaceshipActuator::init( float dt, float time, std::string &error ) {

	thrust = 0.0;
	velocity.cero();

	hasBeenHit = false;

	laser1 = NULL;
	laser2 = NULL;
	laserCol1 = NULL;
	laserCol2 = NULL;
	buildings = NULL;
	progressBarEnergy = NULL;
	progressBarShield = NULL;

	parameters = StarshipData();

	Phase *p = ((Phase *)game);
	if ( ! p ) return true;

	laser1 = new Laser();
	laser2 = new Laser();
	GL1Material *laserMaterial = new GL1Material();
	laserMaterial->illuminated = false;
	laserMaterial->diffuse.set( 0, 0.7, 1 );
	laserMaterial->depthTest = true;
	laser1->material = laserMaterial;
	laser2->material = laserMaterial;

	p->scene->objects.push_back( laser1 );
	p->scene->objects.push_back( laser2 );

	laserCol1 = new LaserCollisionable();
	laserCol1->laser = laser1;
	p->collisionables[ "laser1" ] = laserCol1;
	laserCol2 = new LaserCollisionable();
	laserCol2->laser = laser2;
	p->collisionables[ "laser2" ] = laserCol2;

	buildings = ((Phase *)game)->collisionables[ "buildings" ];

	GL1Material *energyMaterial = new GL1Material();
	energyMaterial->illuminated = false;
	energyMaterial->emission.set( 0.7, 0.7, 0.25 );
	//energyMaterial->specular.set( 0.0, 0.0, 0.0 );

	GL1Material *shieldMaterial = new GL1Material();
	shieldMaterial->illuminated = false;
	shieldMaterial->emission.set( 0.1, 0.25, 0.7 );
	//shieldMaterial->specular.set( 0.0, 0.0, 0.0 );

	progressBarEnergy = new ProgressBar();
	progressBarEnergy->visible = true;
	progressBarEnergy->material = energyMaterial;
	progressBarEnergy->cornerNW.set( 64, 430, - 0.5 );
	progressBarEnergy->cornerSE.set( 0.6, - 0.85, - 0.5 );
	progressBarEnergy->margin = 0.02;
	progressBarEnergy->maxSize = 1.2;
	progressBarEnergy->currentSize = parameters.maxEnergy;
	progressBarEnergy->currentValue = parameters.energy;
	p->scene->objects.push_back( progressBarEnergy );

	progressBarShield = new ProgressBar();
	progressBarShield->visible = true;
	progressBarShield->material = shieldMaterial;
	progressBarShield->cornerNW.set( 64, 460, - 0.5 );
	progressBarShield->cornerSE.set( 0.6, - 0.95, - 0.5 );
	progressBarShield->margin = 0.02;
	progressBarShield->maxSize = 1.2;
	progressBarShield->currentSize = parameters.maxShield;
	progressBarShield->currentValue = parameters.shield;
	p->scene->objects.push_back( progressBarShield );

	radar = new Radar();
	radar->pose = new Pose();
	radar->pose->position.set( 8.7, 8.5, - 20 );
	radar->pose->scale = 2;
	Vector3 axis( 1, 0, 0 );
	axis.normalize();
	radar->pose->rotation.setFromAxisAngle( &axis, M_PI * 0.25 );
	radar->pose->updateMatrix();
	GL1Material *radarMaterial = new GL1Material();
	radarMaterial->illuminated = false;
	radarMaterial->emission.set( 0.9, 0.2, 0 );
	radar->material = radarMaterial;
	p->scene->objects.push_back( radar );

	hull = (GL1Mesh *)(((Group *)spaceshipObject)->objects[ 0 ]);

	condensationLinesLeft = new CondensationLines();
	condensationLinesLeft->pose = spaceshipObject->pose;
	condensationLinesLeft->material = new GL1Material();
	condensationLinesLeft->material->diffuse.set( 1, 1, 1 );
	condensationLinesLeft->material->illuminated = false;
	condensationLinesLeft->material->doubleSided = true;
	condensationLinesLeft->material->depthTest = false;
	condensationLinesLeft->material->minZ = 1;
	condensationLinesLeft->material->maxZ= 1000;
	p->scene->objects.push_back( condensationLinesLeft );

	condensationLinesRight = new CondensationLines();
	condensationLinesRight->pose = spaceshipObject->pose;
	condensationLinesRight->material = new GL1Material();
	condensationLinesRight->material->diffuse.set( 1, 1, 1 );
	condensationLinesRight->material->illuminated = false;
	condensationLinesRight->material->doubleSided = true;
	condensationLinesRight->material->depthTest = false;
	condensationLinesRight->material->minZ = 1;
	condensationLinesRight->material->maxZ= 1000;
	p->scene->objects.push_back( condensationLinesRight );

	exhaust = new Bullet();
	exhaust->radius = 0.7;
	exhaust->pose = new Pose();
	exhaust->material = new GL1Material();
	exhaust->material->diffuse.set( 1, 1, 1 );
	exhaust->material->illuminated = false;
	exhaust->material->doubleSided = true;
	exhaust->material->transparent = true;
	exhaust->material->opacity = 1;
	exhaust->material->texture = GL1Pipeline::loadTexture( SPACESHIP_MADNESS_DIR + std::string( "textures/exhaust.sprite" ), error );
	exhaust->material->depthTest = false;
	exhaust->material->minZ = 7.0f;
	exhaust->material->maxZ = 1500.0f;
	if ( ! exhaust->material->texture ) return false;

	//p->scene->objects.push_back( exhaust );
	((Group *)spaceshipObject)->objects.push_back( exhaust );

	hud = new HUD();
	hud->pose = new Pose();
	hud->material = new GL1Material();
	hud->material->diffuse.set( 1, 0.7, 0 );
	hud->material->illuminated = false;
	hud->material->doubleSided = true;
	hud->visible = false;
	hud->material->depthTest = true;

	/*
	enemyTarget = NULL;
	targettingActivated = false;
	timeTargettingChanged = 0;
	targetCentered = false;
	*/

	p->scene->objects.push_back( hud );

	return true;

}

void SpaceshipActuator::actuate( float dt, float time ) {

	SpaceGamePhase  *phase1 = (SpaceGamePhase *)game;
	Controller *controller = phase1->controller;

	// Update targetting system and HUD
/*
	if ( controller->c ) {
		if ( time > timeTargettingChanged + 0.3 ) {
			if ( targettingActivated ) deactivateTargetting();
			else activateTargetting();
			timeTargettingChanged = time;
		}
	}

	if ( controller->d && enemyTarget ) {
		if ( time > timeTargettingChanged + 0.3 ) {
			enemyTarget = selectNextTarget();
			timeTargettingChanged = time;
		}
	}

	if ( enemyTarget ) {

		SpaceGamePhase *phase1 = (SpaceGamePhase *)game;
		bool found = false;

		if ( enemyTarget->name != std::string( "x" ) ) {

			for ( int32_t i = 0, n = phase1->enemies.size(); i < n; i ++ ) {
				if ( enemyTarget == phase1->enemies[ i ] ) {
					found = true;
					break;
				}
			}
		}

		if ( ! found ) {
			enemyTarget = selectNextTarget();
			if ( enemyTarget ) found = true;
			else deactivateTargetting();
		}
		if ( found) {

			FPObject3D *object = (FPObject3D *)enemyTarget->getObject();
			if ( object->pose->position.distanceTo( &spaceshipObject->pose->position ) > 1000 ) {
				deactivateTargetting();
			} else {
				hud->radius = object->radius;
				hud->pose->position.copy( &object->pose->position );
				hud->pose->updateMatrix();
			}
		}
	}
*/

	// Rotate the ship with the controller
/*
	println( "_-----------------" );
	println( std::to_string( controller->x1 ) );
	println( std::to_string( controller->y1 ) );
*/
	Vector3 joy( controller->x1, controller->y1, 0 );
	float value = hasBeenHit ? 0 : joy.length();
	Vector3 axis( joy.y, - joy.x, 0 );
	axis.normalize();

	Quaternion q1, q2;
	q1.setFromAxisAngle( &axis, value * dt );

	if ( controller->left || controller->right ) {
		// Roll
		Quaternion q3;
		Vector3 rollAxis( 0, 0, -1 );
		q3.setFromAxisAngle( &rollAxis, ( controller->left ? -1 : 1 ) * 18 * dt );
		q1.interpolateQuaternions( &q1, &q3, 0.1 );
	}

	if ( controller->l ) {

		// Stabilization
		Vector3 horizontalAxis( 1, 0, 0 );
		Vector3 directionWorld = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &horizontalAxis );
		Vector3 horizonWorld( directionWorld.x, 0, directionWorld.z );
		float angle = acos( horizonWorld.dotProduct( &directionWorld ) ) * dt * 35;
		if ( directionWorld.y < 0 ) angle = - angle;

		Quaternion horizontalQ;
		Vector3 zAxis( 0, 0, -1 );
		horizontalQ.setFromAxisAngle( &zAxis, angle );
		q1.interpolateQuaternions( &q1, &horizontalQ, 0.1 );
	}

/*
	if ( enemyTarget && ! targetCentered ) {

		// Point to target
		Vector3 horizontalAxis( 1, 0, 0 );
		//Vector3 verticalAxis( 0, 1, 0 );
		Vector3 horDirectionWorld = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &horizontalAxis );
		horDirectionWorld.normalize();
		//Vector3 vertDirectionWorld = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &verticalAxis );
		//vertDirectionWorld.normalize();

		FPObject3D *object = (FPObject3D *)enemyTarget->getObject();
		Vector3 relPos;
		relPos.sub( &object->pose->position, &spaceshipObject->pose->position )->normalize();

		if ( horDirectionWorld.dotProduct( &relPos ) < 0.008 ) targetCentered = true;

		Vector3 vecNorm;
		vecNorm.crossProduct( &horDirectionWorld, &relPos );

		Quaternion rotQ;
		rotQ.setFromAxisAngle( &vecNorm, - dt * 20 );
		q1.interpolateQuaternions( &q1, &rotQ, 0.2 );
	}
*/

	q2 = camera->pose->rotation;
	camera->pose->rotation.multiplyQuaternions( &q2, &q1 );

	q2 = spaceshipObject->pose->rotation;
	spaceshipObject->pose->rotation.multiplyQuaternions( &q2, &q1 );
	spaceshipObject->pose->updateMatrix();

	// Make the camera follow the ship

	q1 = camera->pose->rotation;
	q2.identity();
	camera->pose->rotation.interpolateQuaternions( &q1, &q2, minim( 1.0, dt * 2 ) );

	camera->pose->updateMatrix();
	Matrix4 m( camera->pose->matrix );
	Matrix4 m2;
	m2.inverseHomogeneous( &m );
	camera->pose->matrix.multiply( &spaceshipObject->pose->matrix, &m2 );

	// Thrust / retro engines

	float targetThrust = controller->up;
	if ( targetThrust == 0 ) targetThrust = - controller->down;

//	if ( enemyTarget && ! targetCentered ) targetThrust = 0;

	thrust = targetThrust * thrustRate + thrust * ( 1 - thrustRate );

	// Targetting strafe
/*
	Vector3 targettingPos;
	if ( enemyTarget ) {

		if ( ! targetCentered ) {
			thrust = 0;
			velocity.multiplyScalar( 0.9 );
			targettingPos.copy( &spaceshipObject->pose->position );
		} else {

			FPObject3D *object = (FPObject3D *)enemyTarget->getObject();
			Vector3 relPos;
			relPos.sub( &object->pose->position, &spaceshipObject->pose->position );
			float r = relPos.length();
			Vector3 zAxis( 0, 0, - 1 );
			Vector3 zAxisWorld;
			zAxisWorld = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &zAxis );
			Vector3 projZ;
			float dot = zAxisWorld.dotProduct( &relPos );
			if ( dot >= 0.1 ) {
				projZ.copy( &zAxisWorld )->normalize()->multiplyScalar( dot );
				targettingPos.copy( &projZ )->negate()->normalize()->multiplyScalar( r );
				float l = targettingPos.length();
				if ( l < 3 ) targettingPos.cero();
				targettingPos.inc( &object->pose->position );
			} else targettingPos.copy( &spaceshipObject->pose->position );

		}

	} else targettingPos.copy( &spaceshipObject->pose->position );
*/
	// Dynamics

	float targetVelocity = thrust * maxVelocity;
	Vector3 targetVelocityVector = spaceshipObject->pose->matrix.getDirectorVectorZ();
	targetVelocityVector.negate()->multiplyScalar( targetVelocity );

	if ( hasBeenHit ) {
		velocity.y -= 9.8 * dt;
		if ( time > timeOfHit + 3.0 ) {
			spaceshipObject->visible = false;
		} else {
			Group *g = (Group *)spaceshipObject;
			addRumble(1.0f);
			laserobj->visible = false;
			linesobj->visible = false;
			for ( int32_t i = 0, n = g->objects.size() - 2; i < n; i ++ ) {
				SpaceGamePhase::explodeMeshRandomly( g->objects[ i ]->mesh, 0.03, dt * 0.4 );
			}
		}

	} else {
		velocity.lerp( &velocity, &targetVelocityVector, dt * 0.999 );
	}

	Vector3 increm( velocity );
	increm.multiplyScalar( dt );

	Vector3 *p = &spaceshipObject->pose->position;

	deltaPos.copy( p );

//	if ( enemyTarget && targetCentered ) p->lerp( p, &targettingPos, 4 * dt );
	p->inc( &increm );

	deltaPos.negate()->inc( p );

	// Collisions

	// Collision with ground

	const float bigDelta = 0.1;
	if ( p->y < 0.0 ) {
		p->y = bigDelta;
		velocity.y *= - 0.4;
	}

	if ( p->y > phase1->ceilingHeight ) {
		p->y = phase1->ceilingHeight - bigDelta;
		velocity.y *= - 0.4;
	}

	// Collision with buildings

	if ( buildings ) {
		if ( buildings->isPointInside( p ) ) {
			if ( velocity.length() > 20.0 ) {
				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "collision" ) ) ) {
					println( "Could not play collision" );
				}
			}
			velocity.cero();
			thrust = 0;
			p->dec( &increm );
			deltaPos.cero();
		}
	}

	spaceshipObject->pose->updateMatrix();

	// Update grids poses
	if ( phase1->floorGrid ) {
		phase1->floorGrid->pose->position.copy( p );
		phase1->floorGrid->pose->updateMatrix();
	}
	if ( phase1->ceilingGrid ) {
		phase1->ceilingGrid->pose->position.copy( p );
		phase1->ceilingGrid->pose->updateMatrix();
	}

	// Update lasers

	if ( ! hasBeenHit && controller->z ) {

		parameters.energy -= dt * 0.15 * 0.5;
		if ( parameters.energy >= 0 ) {
			if(!laserobj->visible){
				if ( ! ((SpaceGamePhase *)game)->sound->playAudio( std::string( "laser" ), 0.6 ) ) {
					println( "Could not play laser" );
				} else addRumble(0.3f);
			}
			updateLaser( laser1, false );
			laserobj->visible = true;
		}
		else {
			laserobj->visible = false;
			laser1->visible = false;
			parameters.energy = 0;
		}

		parameters.energy -= dt * 0.15 * 0.5;
		if ( parameters.energy >= 0 ) {
			updateLaser( laser2, true );
			laserobj->visible = true;
		}
		else {
			laserobj->visible = false;
			laser2->visible = false;
			parameters.energy = 0;
		}

	} else {
		laser1->visible = false;
		laser2->visible = false;
		laserobj->visible = false;
		laserobj->visible = false;

	}

	// Energy consumption / regeneration

	if ( ! hasBeenHit ) parameters.energy += parameters.energyRegeneration * dt;
	if ( parameters.energy < 0 ) parameters.energy = 0;
	else if ( parameters.energy > parameters.maxEnergy ) parameters.energy = parameters.maxEnergy;

	// Shield regeneration

	if ( ! hasBeenHit ) {
		float regeneration = parameters.shieldRegeneration * dt;
		parameters.energy -= regeneration;
		if ( parameters.energy >= 0 ) {
			parameters.shield += regeneration;
			if ( parameters.shield > parameters.maxShield ) {
				parameters.energy += parameters.shield - parameters.maxShield;
				parameters.shield = parameters.maxShield;
			}
		} else parameters.energy = 0;
	}
	if ( parameters.shield > parameters.maxShield ) parameters.shield = parameters.maxShield;

	hull->material->diffuse.g = minim( 0.8, hull->material->diffuse.g + 0.2 * dt );
	hull->material->diffuse.b = hull->material->diffuse.g;

	// Update energy and shield indicators

	progressBarEnergy->currentValue = parameters.energy;
	progressBarEnergy->cornerSE.x = progressBarEnergy->cornerNW.x + progressBarEnergy->currentSize * progressBarEnergy->maxSize;
	progressBarShield->currentValue = parameters.shield;
	progressBarShield->cornerSE.x = progressBarShield->cornerNW.x + progressBarShield->currentSize * progressBarShield->maxSize;

	// Update radar markings

	radar->points.clear();

	Vector3 vMinusOne( -1, -1, -1 );
	Vector3 vOne( 1, 1, 1 );

	for ( int32_t i = 0, n = phase1->enemies.size(); i < n; i ++ ) {

		Actuator *act = phase1->enemies[ i ];
		FPObject3D *object = (FPObject3D *)act->object;
		Vector3 pos;
		pos.sub( &object->pose->position, p );
		pos.multiplyScalar( 1 / radar->range );
		pos = spaceshipObject->pose->matrix.vector3FreeXMatrix4Inverse( &pos );
		pos.clamp( &vMinusOne, &vOne );
		RadarPoint rp;
		rp.position.copy( &pos );
		/*
		if ( targettingActivated && enemyTarget == act ) {
			rp.color.set( 0.95, 0.95, 0 );
		} else {
			rp.color.set( 0.95, 0.2, 0 );
		}
		*/
		rp.color.set( 0.95, 0.2, 0 );
		radar->points.push_back( rp );
	}

	for ( int32_t i = 0, n = phase1->powerups.size(); i < n; i ++ ) {

		if ( ! phase1->powerups[ i ]->enabled ) continue;
		FPObject3D *object = (FPObject3D *)( phase1->powerups[ i ]->object );
		Vector3 pos;
		pos.sub( &object->pose->position, p );
		pos.multiplyScalar( 1 / radar->range );
		pos = spaceshipObject->pose->matrix.vector3FreeXMatrix4Inverse( &pos );
		pos.clamp( &vMinusOne, &vOne );
		RadarPoint rp;
		rp.position.copy( &pos );
		rp.color.set( 0, 1, 1 );
		radar->points.push_back( rp );
	}

	// Update condensation trail lines

	//condensationLinesLeft->trailPoints[ 0 ].set( - 6.5, - 0.21, 4.62 );
	//Vector3 velObj;
	//velObj = spaceshipObject->pose->matrix.vector3FreeXMatrix4Inverse( &velocity );
	//condensationLinesLeft->trailPoints[ 1 ].set( - 6.5 + velObj.x * 0.025, - 0.21 + velObj.y * 0.025, 25 );
	float v = velocity.length();
	//condensationLinesLeft->visible = v > 200;
	this->linesobj->visible = v > 200;
	//condensationLinesLeft->thickness = 1.0 * maxim( 0, v - 200.0 ) / 200.0;

	//condensationLinesRight->trailPoints[ 0 ].set( 6.5, - 0.21, 4.62 );
	//velObj = spaceshipObject->pose->matrix.vector3FreeXMatrix4Inverse( &velocity );
	//condensationLinesRight->trailPoints[ 1 ].set( 6.5 + velObj.x * 0.025, - 0.21 + velObj.y * 0.025, 25 );
	//condensationLinesRight->visible = condensationLinesLeft->visible;
	//condensationLinesRight->thickness = condensationLinesLeft->thickness;

	// Update exhaust

	Vector3 exhaustPosRel( 0, 0, 6.2 );
	exhaust->pose->position = spaceshipObject->pose->matrix.vector3XMatrix4( &exhaustPosRel );
	exhaust->pose->updateMatrix();

	exhaust->radius = thrust > 0.1 ? thrust * 0.8 + frand() * 0.1 : 0;
	exhaust->visible = ! hasBeenHit;

}

void SpaceshipActuator::updateLaser( Laser *laser, bool isLeftLaser ) {

	Vector3 pos( isLeftLaser ? - 2.46 : 2.46, 0, - 0.25 );
	Vector3 dir( 0, 0, - 1000 );
	Vector3 dirvis( 0, 0, - 100 );
	laser->beginPoint = spaceshipObject->pose->matrix.vector3XMatrix4( &pos );
	laser->ray = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &dir );
	laser->rayvis = spaceshipObject->pose->matrix.vector3FreeXMatrix4( &dirvis );
	laser->visible = true;

	// Collision laser with buildings

	if ( buildings ) {
		float intersectionParam;
		Vector3 intersectionNormal;
		if ( buildings->castRay( &laser->beginPoint, &laser->ray, &intersectionParam, &intersectionNormal ) ) {
			laser->ray.multiplyScalar( intersectionParam );
		}
	}

	// Collision laser with ground

	if ( laser->ray.y < - 0.01 ) {
		Vector3 ur = laser->ray;
		ur.normalize();
		float p =  laser->beginPoint.y / ur.y;
		if ( p < 1 && p > 0.0001 ) laser->ray.multiplyScalar( p );
	}

}

void SpaceshipActuator::finish( float dt, float time ) {
}

void SpaceshipActuator::receiveHit( float amount, float dt, float time ) {

	parameters.shield -= amount;
	if ( parameters.shield < 0 && ! hasBeenHit ) {
		hasBeenHit = true;
		timeOfHit = time;
	}

	hull->material->diffuse.g = 0;
	hull->material->diffuse.b = 0;

}

StarshipData SpaceshipActuator::getStarshipParameters() {
	return parameters;
}

void SpaceshipActuator::setStarshipParameters( StarshipData *parameters ) {

	this->parameters = *parameters;

	if ( progressBarEnergy ) progressBarEnergy->currentSize = parameters->maxEnergy;
	if ( progressBarShield ) progressBarShield->currentSize = parameters->maxShield;

}
/*
void SpaceshipActuator::activateTargetting() {

	enemyTarget = selectNextTarget();

	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;

	if ( enemyTarget ) {
		hud->visible = true;
		phase1->sound->playAudio( "menu1" );
		targettingActivated = true;
		targetCentered = false;
	} else {
		hud->visible = false;
		targettingActivated = false;
		targetCentered = false;
		phase1->sound->playAudio( "menu2" );
	}

}

void SpaceshipActuator::deactivateTargetting() {
	hud->visible = false;
	enemyTarget = NULL;
	targetCentered = false;
	targettingActivated = false;
	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;
	phase1->sound->playAudio( "menu2" );
}

Actuator *SpaceshipActuator::selectNextTarget() {

	float minDist = 10000;
	SpaceGamePhase *phase1 = (SpaceGamePhase *)game;
	Actuator *target = NULL;
	for ( int32_t i =  0, n = phase1->enemies.size(); i < n; i ++ ) {

		Actuator *enemy = phase1->enemies[ i ];

		// Discard non-selectable enemies
		if ( enemy->name.length() > 1 ) continue;

		// Deselect died enemy
		if ( enemy->name == std::string( "x" ) ) continue;

		FPObject3D *object = (FPObject3D *)enemy->getObject();
		float d = spaceshipObject->pose->position.distanceTo( &object->pose->position );
		if ( d < 1000 && d < minDist ) {
			target = enemy;
			targetCentered = false;
			minDist = d;
		}

	}

	return target;

}
*/
