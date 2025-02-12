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

#include <string>
#include <vector>

#include "math3d/MathUtils.h"

#include "graphicsSystem/GraphicsSystem.h"

namespace ceres {

class Simulation {
public:

	Simulation(): graphicsSystem( NULL ) {}
	virtual ~Simulation() { delete graphicsSystem; }

	typedef enum {
		SIM_TERMINATE = -2,
		SIM_CONTINUE = -1
	} SimulationStepResult;

	virtual std::string getTitle() { return std::string( "Ceres" ); }
	virtual unsigned int getBackendVersionMajor() { return 0; }
	virtual unsigned int getBackendVersionMinor() { return 0; }
	virtual unsigned int getDisplayCaps() { return 0; }
	virtual unsigned int getContextFlags() { return 0; }
	virtual bool getInitialScreenPos( int &x, int &y ) { return false; }
	virtual bool getInitialScreenSize( int &width, int &height ) {

		if ( graphicsSystem ) {

			GraphicsMode *graphicsMode = graphicsSystem->getCurrentGraphicsMode();
			width = graphicsMode->resolutionX;
			height = graphicsMode->resolutionY;

			return true;

		}

		return false;

	}
	virtual bool getInitialFullScreen() { return false; }
	virtual bool drawGUIBeforeSimulationStep() { return false; }

	virtual bool init( std::string &error ) { return true; };
	virtual SimulationStepResult timestep( scalar dt, scalar elapsedRealTime ) { return SIM_TERMINATE; };
	virtual float getDisplayDelayMilliseconds() { return 0; };
	virtual void newViewPortSize( int width, int height ) { };
	virtual bool finish( std::string &error ) { return true; };

    virtual bool hasGUI() { return false; }
    virtual SimulationStepResult defineGUI() { return SIM_CONTINUE; }

    GraphicsSystem *graphicsSystem;

};

}
