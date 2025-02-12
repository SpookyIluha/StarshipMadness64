/*
 * Copyright © 2024 yomboprime https://codeberg.org/yombo
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
#include "fixedPipeline/gl1/GL1Text3DScreen.h"
#include "../objects/BannerMesh.h"
#include "ScoreActuator.h"
#include "math3d/Vector3.h"
#include "math3d/Camera.h"

#include "../SpaceshipMadnessConfig.h"

namespace ceres {

class BannerActuator : public Actuator {
public:

	BannerActuator();
	virtual ~BannerActuator();

	virtual bool init( float dt, float time, std::string &error ) override;
	virtual void actuate( float dt, float time ) override;
	virtual void finish( float dt, float time ) override;

	void showWave( int32_t wave, float time );
	void showWellDone( float time );
	void showGameOver( float time );

	void createWave( int32_t wave, float time );

	GL1Mesh *waveTextObject;
	GL1Mesh *gameOverTextObject;
	GL1Mesh *digitsTextObjects[ 10 ];

	int32_t numWavesEmmited;

	static const int32_t BANNER_IDLE = 0;
	static const int32_t BANNER_SHOW_WAVE = 1;
	static const int32_t BANNER_SHOW_WELL_DONE = 2;
	static const int32_t BANNER_SHOW_GAME_OVER = 3;

	GL1Text3DScreen *bonusText;

	BannerMesh *welldoneText;

	int32_t state;
	float time0;

	SpaceshipActuator *spaceshipActuator;
	ScoreActuator *scoreActuator;

};

}
