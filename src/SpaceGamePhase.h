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

#include <vector>
#include <map>

#include "game/Phase.h"
#include "objects/Grid.h"
#include "Sound.h"
#include "actuators/SpaceshipActuator.h"
#include "actuators/EnemyActuator.h"
#include "actuators/BannerActuator.h"
#include "actuators/PowerupActuator.h"
#include "fixedPipeline/gl1/GL1Parallelepiped.h"
#include "fixedPipeline/FPLight.h"
#include "fixedPipeline/gl1/GL1Scene.h"
#include "fixedPipeline/FPFont.h"

namespace ceres {

class SpaceGamePhase : public Phase {
public:

	SpaceGamePhase();
	virtual ~SpaceGamePhase();

	SpaceshipActuator *createSpaceship( bool addActuator, std::string &error );
	static void explodeMeshRandomly( Mesh *mesh, float randomization, float dt );
	PowerupActuator *createPowerup( int32_t powerupType, float dt, float time, std::string &error );
	bool placeObject( Posable *object, Collisionable *collisionable, float extentsRange, float minHeight, float maxHeight, Vector3 offset );
	void setDefaultLight( Scene *scene );

	virtual void createEnemiesWave( int32_t wave, float time ) {}

	bool terminationRequested;

	bool drawFloor;
	bool drawCeiling;
	float ceilingHeight;
	GraphicsColor floorColor;
	GraphicsColor ceilingColor;

	// Grid variables
	float squareLength;
	float squareLineWidth;

	// Grid culling
	float zViewDistance;

	// Grids
	Grid *floorGrid;
	Grid *ceilingGrid;

	// Size in meters of the phase's region of interest
	float citySize;

	// Game objects
	SpaceshipActuator *spaceshipActuator;
	std::vector<Actuator *> enemies;
	std::vector<PowerupActuator *> powerups;

	ScoreActuator *scoreActuator;

	// Actuator that controls on-screen banners, enemies waves and phase termination
	BannerActuator *bannerActuator;

	// Number of enemies waves in this phase, or 0 for special phases
	int32_t numWaves;

	// Other common game variables
	bool allowCreatePowerup;
	float nextTimeCreatePowerup;

	// Sound system
	Sound *sound;

	// Main font
	FPFont *font1;

	bool canPause;

	bool menuIsShown;

};

}
