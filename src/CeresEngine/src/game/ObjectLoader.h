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

#include "math3d/Group.h"
#include "game/Actuator.h"
#include "fixedPipeline/gl1/GL1Pipeline.h"

namespace ceres {

class ObjectLoader {
public:

	ObjectLoader();
	virtual ~ObjectLoader();

	bool createY3DObject( Actuator *actuator, GL1Pipeline *pipeline, std::string modelPath, std::string texturePathPrefix, Vector3 position, float scale, std::string &error );
	std::string readLine( uint8_t *buffer, uint64_t &cursor, uint64_t size );
	int32_t readInteger( uint8_t *buffer, uint64_t &cursor, uint64_t size );
	float readFloat( uint8_t *buffer, uint64_t &cursor, uint64_t size );

};

}
