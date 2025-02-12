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
#include "SpaceshipActuator.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "game/collision/BoxCollisionable.h"
#include "game/collision/RadiusCollisionable.h"

namespace ceres {

struct Enemy3Ball {

	Enemy3Ball() {
	}

	virtual ~Enemy3Ball() {
	}

	float health;
	FPObject3D *object;
	Collisionable *collisionable;
};

class Enemy3Actuator : public Actuator {
public:

	Enemy3Actuator();
	virtual ~Enemy3Actuator();

	virtual bool init( float dt, float time, std::string &error ) override;
	virtual void actuate( float dt, float time ) override;
	virtual void finish( float dt, float time ) override;

	Vector3 initialPosition;

	GL1Mesh *rodObject;
	GL1Mesh *bodyObject;
	Enemy3Ball balls[ 4 ];

	float targetY;
	float speed;

	BoxCollisionable *rodCollisionable;
	RadiusCollisionable *bodyCollisionable;

	bool isDestroying;
	float timeToDestroy;

	SpaceshipActuator *spaceshipActuator;
	Collisionable *starship;
	Collisionable *laser1;
	Collisionable *laser2;

};

}
