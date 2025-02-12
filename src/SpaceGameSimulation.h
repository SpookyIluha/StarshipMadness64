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

#include "ceres/Simulation.h"

#include "SpaceshipMadnessConfig.h"

#include "SpaceGamePhase.h"
#include "actuators/SpaceshipActuator.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "Menu.h"

#include "Sound.h"

#include "console/console.h"
#include "utils/utils.h"

#include <dc/maple.h>
#include <dc/maple/controller.h>

using namespace ceres;

class SpaceGameSimulation: public Simulation {
public:

	SpaceGameSimulation() {}

	virtual std::string getTitle() override;

	virtual bool init( std::string& error ) override;

	virtual SimulationStepResult timestep( scalar dt, scalar elapsedRealTime ) override;

	// Not used
	//virtual void newViewPortSize( int width, int height ) override;

	virtual bool finish( std::string &error ) override;

	bool loadAudios( std::string &error );
	FPFont *createFont1( std::string  &error );

	int32_t getNumPhases();
	SpaceGamePhase *createPhase( int32_t index );

	void drawScreenSaver( float dt );

	GL1Pipeline pipeline;

	int32_t currentPhaseIndex;
	SpaceGamePhase *currentPhase;
	int32_t selectedPhaseIndex;

	Controller controller;
	maple_device_t *mapleController;
	cont_state_t *controllerState;

	float theTime;

	Sound *sound;

	Menu *menu;

	FPFont *font1;

	bool screenSaver;
	float timeLastKeypress;

	Scene *screenSaverScene;
	Camera *screenSaverCamera;
	GL1Text3D *screenSaverText;
	Vector3 screenSaverPos;
	Vector3 screenSaverDirection;

};
