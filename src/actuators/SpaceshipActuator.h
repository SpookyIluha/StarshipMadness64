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

#pragma once

#include <string>
#include <dc/maple/controller.h>
#include "game/Actuator.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "math3d/Vector3.h"
#include "math3d/Camera.h"
#include "../objects/Laser.h"
#include "../collisionGame/LaserCollisionable.h"
#include "../objects/ProgressBar.h"
#include "../objects/Radar.h"
#include "../objects/Bullet.h"
#include "../objects/HUD.h"
#include "../objects/CondensationLines.h"

namespace ceres {

struct StarshipData {

	StarshipData() :
		shield( 0.4 ),
		energy( 0.4 ),
		maxShield( 0.4 ),
		maxEnergy( 0.4 ),
		shieldRegeneration( 0.02 ),
		energyRegeneration( 0.05 ),
		score( 0 ),
		hiScore( 0 ) {
	}
	virtual ~StarshipData() {}

	void setDefaultValues() {
		shield = 0.4;
		energy = 0.4;
		maxShield = 0.4;
		maxEnergy = 0.4;
		shieldRegeneration = 0.02;
		energyRegeneration = 0.05;
		score = 0;
	}

	float shield;
	float energy;
	float maxShield;
	float maxEnergy;
	float shieldRegeneration;
	float energyRegeneration;
	int32_t score;
	int32_t hiScore;
};

class SpaceshipActuator : public Actuator {
public:

	SpaceshipActuator();
	virtual ~SpaceshipActuator();

	virtual bool init( float dt, float time, std::string &error ) override;
	virtual void actuate( float dt, float time ) override;
	virtual void finish( float dt, float time ) override;

	void receiveHit( float amount, float dt, float time );
	void updateLaser( Laser *laser, bool isLeftLaser );

	StarshipData getStarshipParameters();
	void setStarshipParameters( StarshipData *parameters );

	void activateTargetting();
	void deactivateTargetting();
	Actuator *selectNextTarget();

	Object3D *spaceshipObject;
	GL1Mesh *hull;
	Camera *camera;

	Laser *laser1;
	LaserCollisionable *laserCol1;
	Laser *laser2;
	LaserCollisionable *laserCol2;

	Collisionable *buildings;
	Collisionable *enemies;

	ProgressBar *progressBarEnergy;
	ProgressBar *progressBarShield;

	Radar *radar;

	CondensationLines *condensationLinesLeft;
	CondensationLines *condensationLinesRight;

	Bullet *exhaust;

	HUD *hud;
/*
	Actuator *enemyTarget;
	bool targettingActivated;
	float timeTargettingChanged;
	bool targetCentered;
*/

	const float maxVelocity = 300.0;
	float thrustRate = 0.01;

	float targetVelocity;
	Vector3 velocity;
	float thrust;

	StarshipData parameters;

	float energyRegeneration;
	float shieldRegeneration;

	bool hasBeenHit;
	float timeOfHit;

	Vector3 deltaPos;

};

}
