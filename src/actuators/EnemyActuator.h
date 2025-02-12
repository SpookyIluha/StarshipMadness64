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
#include "game/Actuator.h"
#include "SpaceshipActuator.h"
#include "fixedPipeline/gl1/GL1Mesh.h"

#include "../SpaceshipMadnessConfig.h"

namespace ceres {

class EnemyActuator : public Actuator {
public:

	EnemyActuator();
	virtual ~EnemyActuator();

	virtual bool init( float dt, float time, std::string &error ) override;
	virtual void actuate( float dt, float time ) override;
	virtual void finish( float dt, float time ) override;

	void receiveHit( float amount, float dt, float time );

	// 1 or 2
	int32_t enemyType;

	float health;
	float maxVelocity;

	GL1Mesh *enemyObject;
	Vector3 velocity;

	Vector3 randomDirection;

	float timeToDestroy;

	float timeForNextBulletShot;

	bool enabled;

	SpaceshipActuator *spaceshipActuator;
	Collisionable *buildings;
	Collisionable *starship;
	Collisionable *laser1;
	Collisionable *laser2;

};

}
