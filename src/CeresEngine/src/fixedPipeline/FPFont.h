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

#include <vector>
#include <map>
#include "../math3d/Vector2.h"
#include "FPTexture.h"

namespace ceres {

class FPFont {
public:

	FPFont();
	virtual ~FPFont();

	// TODO: Creation functions for other alphabets
	void initWesternCharset();

	void generateUVs( std::vector<Vector2> characterSizes, FPTexture *texture );

	int32_t fontHeight;

	// Character set
	std::vector<std::string> charset;

	// Map characters to character indexes
	std::map<std::string, int32_t> characterIndexes;

	// Map character indexes to UV
	std::vector<Vector2> characterUV0;
	std::vector<Vector2> characterUV1;

	// Character widths in pixels
	std::vector<Vector2> characterSizes;

	// Newline character index
	int32_t newlineIndex;

	// The charset texture
	FPTexture *texture;

};

}
