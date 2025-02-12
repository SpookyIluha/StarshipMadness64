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

#include "Vector3.h"
#include "Mesh.h"

namespace ceres {

class AABBox {
public:

	AABBox();
	~AABBox();

	void reset();
	void set( Vector3 *origin, Vector3 *extents );

	void expandByPoint( Vector3 *point );
	void expandByMesh( Mesh *mesh );
	void expandByAABBox( AABBox *box );
	void expandByScalar( scalar s );

	bool isPointInside( Vector3 *p );

	Vector3 getExtents();
	Vector3 getHalfSize();
	Vector3 getCenter();
	Vector3 getCorner0();
	Vector3 getCorner1();

	bool isReset;
	Vector3 corner0, corner1;

};

}
