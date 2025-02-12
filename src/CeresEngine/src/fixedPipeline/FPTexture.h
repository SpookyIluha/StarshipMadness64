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

namespace ceres {

class FPTexture {
public:

	FPTexture();
	virtual ~FPTexture();

	virtual bool init( int32_t resolutionX, int32_t resolutionY, int32_t numComponents, bool magLinearFilter, bool minLinearFilter, bool mipmap, void *data, std::string &error );

	static float *createBuffer( int32_t resolutionX, int32_t resolutionY, int32_t numComponents );

	virtual void upload( void *src );

	static int32_t getPixelSizeBytes( int32_t numComponents );

	int32_t numComponents;

	int32_t resolutionX;
	int32_t resolutionY;

	bool magLinearFilter;
	bool minLinearFilter;
	bool hasMipmap;
	bool repeat;

	int32_t bufferSizeBytes;
	float *data;

protected:

private:

};

}
