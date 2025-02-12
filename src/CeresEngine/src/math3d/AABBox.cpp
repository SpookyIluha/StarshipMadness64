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

#include "AABBox.h"

using namespace ceres;

AABBox::AABBox() {
	reset();
}


AABBox::~AABBox() {}

void AABBox::reset() {
	corner0.cero();
	corner1.cero();
	isReset = true;
}

void AABBox::set( Vector3 *origin, Vector3 *extents ) {
	corner0 = origin;
	corner1.copy( extents );
	if ( corner1.x < 0 ) corner1.x = 0;
	if ( corner1.y < 0 ) corner1.y = 0;
	if ( corner1.z < 0 ) corner1.z = 0;
	corner1.inc( origin );
	isReset = false;
}

void AABBox::expandByPoint( Vector3 *point ) {

	if ( isReset ) {

		corner0 = *point;
		corner1 = *point;

	}
	else {

		if ( point->x < corner0.x ) corner0.x = point->x;
		if ( point->y < corner0.y ) corner0.y = point->y;
		if ( point->z < corner0.z ) corner0.z = point->z;
		if ( point->x > corner1.x ) corner1.x = point->x;
		if ( point->y > corner1.y ) corner1.y = point->y;
		if ( point->z > corner1.z ) corner1.z = point->z;

	}

	isReset = false;

}

void AABBox::expandByMesh( Mesh *mesh ) {

	int32_t attributeStride = Mesh::computeAttributeSizeFloats( mesh->attributes );
	int32_t il = mesh->numVertices * attributeStride;

	if ( il == 0 ) return;

	int32_t i = 0;

	if ( isReset ) {

		Vector3 point;
		point.copyFromArray( mesh->vertices, i );
		expandByPoint( &point );
		i += attributeStride;

	}

	for ( ; i < il; i += attributeStride ) {

		Vector3 point;
		point.copyFromArray( mesh->vertices, i );

		if ( point.x < corner0.x ) corner0.x = point.x;
		if ( point.y < corner0.y ) corner0.y = point.y;
		if ( point.z < corner0.z ) corner0.z = point.z;
		if ( point.x > corner1.x ) corner1.x = point.x;
		if ( point.y > corner1.y ) corner1.y = point.y;
		if ( point.z > corner1.z ) corner1.z = point.z;

	}

}

void AABBox::expandByAABBox( AABBox *box ) {

	if ( box->isReset ) return;

	expandByPoint( &box->corner0 );
	expandByPoint( &box->corner1 );

}

void AABBox::expandByScalar( scalar s ) {

	if ( isReset ) return;

	corner0.x -= s;
	corner0.y -= s;
	corner0.z -= s;
	corner1.x += s;
	corner1.y += s;
	corner1.z += s;

}

Vector3 AABBox::getCenter() {

	Vector3 center = getHalfSize();
	center.inc( &corner0 );
	return center;

}

Vector3 AABBox::getCorner0() {

	return corner0;

}

Vector3 AABBox::getCorner1() {

	return corner1;

}

Vector3 AABBox::getExtents() {
	Vector3 extents( &corner1 );
	extents.dec( &corner0 );
	return extents;
}

Vector3 AABBox::getHalfSize() {

	Vector3 halfSize = getExtents();
	halfSize.multiplyScalar( 0.5 );
	return halfSize;

}

bool AABBox::isPointInside( Vector3 *p ) {

	return p->x >= corner0.x && p->x <= corner1.x &&
		p->y >= corner0.y && p->y <= corner1.y &&
		p->z >= corner0.z && p->z <= corner1.z;

}
