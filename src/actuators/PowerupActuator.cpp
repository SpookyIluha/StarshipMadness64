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

#include "PowerupActuator.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"

#include "game/Phase.h"
#include "../SpaceGamePhase.h"
#include "console/console.h"

using namespace ceres;

PowerupActuator::PowerupActuator() {
}

PowerupActuator::~PowerupActuator(){
}

bool PowerupActuator::init( float dt, float time, std::string &error ) {

	enabled = true;

	GL1ObjectUtils objectUtils;

	GL1Material *powerupMaterial = new GL1Material();
	std::string path = "";

	switch ( powerupType ) {
		case POWERUP_MAX_ENERGY:
			powerupMaterial->diffuse.set( 0.7, 0.3, 0.0 );
			path = SPACESHIP_MADNESS_DIR + std::string( "models/charset/energy.stl" );
			break;
		case POWERUP_REG_ENERGY:
			powerupMaterial->diffuse.set( 0.8, 0.6, 0.6 );
			path = SPACESHIP_MADNESS_DIR + std::string( "models/charset/energy.stl" );
			break;
		case POWERUP_MAX_SHIELD:
			powerupMaterial->diffuse.set( 0.0, 0.1, 1.0 );
			path = SPACESHIP_MADNESS_DIR + std::string( "models/charset/shield.stl" );
			break;
		case POWERUP_REG_SHIELD:
			powerupMaterial->diffuse.set( 0.6, 0.6, 0.8 );
			path = SPACESHIP_MADNESS_DIR + std::string( "models/charset/shield.stl" );
			break;
		default:
			break;
	}
	powerupMaterial->specular.set( 0.0, 0.0, 0.0 );

	Vector3 pos( 0, 0, 0 );
	object = objectUtils.createObject( path, powerupMaterial, 40.0, pos, error, new GL1Mesh(), 8.0 );
	if ( ! object ) {
		error = std::string( "Couldn't load powerup mesh: " ) + path;
		return false;
	}

	Phase *phase1 = (Phase *)game;

	starship = phase1->collisionables[ "starship" ];

	if ( phase1->scene != NULL ) phase1->scene->objects.push_back( object );

	phase1->actuatorsToBeAdded.push_back( this );


	return true;

}

void PowerupActuator::actuate( float dt, float time ) {

	SpaceGamePhase *phase1 = ((SpaceGamePhase *)game);

	// Collision with the ship
	if ( starship ) {
		if ( starship->intersectsSphere( &object->pose->position, object->radius ) ) {

			if ( ! phase1->sound->playAudio( std::string( "powerup" ), 0.7 ) ) {
				println( "Could not play powerup sound." );
			}

			if ( spaceshipActuator ) {
				switch ( powerupType ) {
					case POWERUP_MAX_ENERGY:
						spaceshipActuator->parameters.maxEnergy = minim( spaceshipActuator->parameters.maxEnergy + 0.1, 1.0 );
						spaceshipActuator->progressBarEnergy->currentSize = spaceshipActuator->parameters.maxEnergy;
						break;
					case POWERUP_REG_ENERGY:
						spaceshipActuator->parameters.energyRegeneration *= 2.0;
						spaceshipActuator->parameters.energyRegeneration = minim( spaceshipActuator->parameters.energyRegeneration, 10.0 );
						break;
					case POWERUP_MAX_SHIELD:
						spaceshipActuator->parameters.maxShield = minim( spaceshipActuator->parameters.maxShield + 0.1, 1.0 );
						spaceshipActuator->progressBarShield->currentSize = spaceshipActuator->parameters.maxShield;
						break;
					case POWERUP_REG_SHIELD:
						spaceshipActuator->parameters.shieldRegeneration *= 2.0;
						spaceshipActuator->parameters.shieldRegeneration = minim( spaceshipActuator->parameters.shieldRegeneration, 10.0 );
						break;
					default:
						break;
				}
			}

			spaceshipActuator->parameters.score += 1000;

			phase1->allowCreatePowerup = true;
			phase1->actuatorsToBeDeleted.push_back( this );

			enabled = false;
			std::vector<PowerupActuator *> &powerups = ((SpaceGamePhase *)game)->powerups;
			for ( uint32_t i = 0, n = powerups.size(); i < n; i ++ ) {
				if ( ( powerups[ i ] ) == this ) {
					powerups.erase( powerups.begin() + i );
					break;
				}
			}

		}
	}

	// Powerup rotation
	Vector3 yAxis( 0, 1, 0 );
	object->pose->rotation.setFromAxisAngle( &yAxis, time * 2.5 );
	object->pose->updateMatrix();

}

void PowerupActuator::finish( float dt, float time ) {

	((Phase *)game)->scene->deleteObject( object );

}
