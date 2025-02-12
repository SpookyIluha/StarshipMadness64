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

#include "FPObject3D.h"
#include "FPMaterial.h"
#include "math3d/Mesh.h"

namespace ceres {

class FPMesh: public FPObject3D {
public:

	FPMesh();
	virtual ~FPMesh();

	virtual bool init( Mesh *mesh, FPMaterial *material ) { return false; };

	virtual void setupRender( Camera *camera ) override;
	virtual void finishRender() override;

private:

};

}
