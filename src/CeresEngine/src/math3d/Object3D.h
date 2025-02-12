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

#include "math3d/Posable.h"
#include "math3d/Camera.h"
#include "math3d/Mesh.h"
#include <libdragon.h>

namespace ceres {

class Object3D : public Posable {
public:

	Object3D();
	virtual ~Object3D();

	virtual void setupRender( Camera *camera ) {};
	virtual void finishRender() {};
	virtual void render( Camera *camera ) {};

	bool visible;

	// If 0 it means infinite
	float radius;

	// Optional mesh
	Mesh *mesh;

private:

};

}
