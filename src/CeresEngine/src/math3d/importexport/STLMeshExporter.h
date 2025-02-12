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

#include "inttypes.h"
#include <string>

#include "math3d/Mesh.h"

namespace ceres {

class STLMeshExporter {

public:

	static bool exportSTL( Mesh * mesh, std::string path, float scale, std::string &error );

private:

	static bool writeSTLHeader( FILE *f, std::string path, std::string &error );
	static bool writeFloat( FILE *f, std::string path, float value, std::string &error );
	static bool writeUInt32( FILE *f, std::string path, uint32_t value, std::string &error );
	static bool writeUInt16( FILE *f, std::string path, uint16_t value, std::string &error );

};

}
