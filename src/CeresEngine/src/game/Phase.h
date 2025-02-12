/*
 * Copyright © 2020 - 2025 yomboprime https://codeberg.org/yombo
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

#include <map>

#include "math3d/Scene.h"
#include "fixedPipeline/FPObject3D.h"
#include "Actuator.h"
#include "collision/Collisionable.h"

namespace ceres {

class Phase {
public:

	Phase();
	virtual ~Phase();

	virtual bool initPhase( std::string &error );
	virtual void terminatePhase();

	virtual void timestep( float dt, float time ) {}

	GraphicsColor backgroundColor;
	Scene *scene;
	Camera *camera;

	std::vector<Actuator *> actuators;
	std::vector<Actuator *> actuatorsToBeDeleted;
	std::vector<Actuator *> actuatorsToBeAdded;
	std::map<std::string, Collisionable *> collisionables;

	Controller *controller;

	bool isTerminated;
	bool isSucceeded;

};

}
