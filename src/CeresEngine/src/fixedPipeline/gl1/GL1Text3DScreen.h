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
#include <string>
#include <map>

#include "math3d/Vector2.h"
#include "math3d/Camera.h"
#include "fixedPipeline/gl1/GL1Text3D.h"
#include "fixedPipeline/FPObject3D.h"
#include "fixedPipeline/FPTexture.h"
#include "fixedPipeline/FPFont.h"

namespace ceres {

class GL1Text3DScreen : public GL1Text3D {
public:

	GL1Text3DScreen();
	virtual ~GL1Text3DScreen();

	virtual void setupRender( Camera *camera ) override;
	virtual void finishRender() override;

};

}
