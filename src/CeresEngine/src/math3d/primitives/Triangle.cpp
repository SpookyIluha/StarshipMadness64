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

#include "Triangle.h"

using namespace ceres;

Triangle::Triangle() {
}


Triangle::~Triangle() {
}

Mesh *Triangle::createTriangle( Vector3 v0, Vector3 v1, Vector3 v2, std::vector<Attribute> attributes, Matrix4 transform ) {

	int32_t numFloatsVertex = Mesh::computeAttributeSizeFloats( attributes );

	Mesh *mesh = new Mesh();
	mesh->numVertices = 3;
	mesh->numIndices = 0;
	mesh->vertices = new float[ mesh->numVertices * numFloatsVertex ];
	mesh->indices = NULL;
	mesh->attributes = attributes;
	mesh->primitive = Mesh::MESH_TRIANGLES;

	int32_t p = 0;

	Vector3 pos;
	pos = transform.vector3XMatrix4( &v0 );
	pos.copyToArray( mesh->vertices, p );
	Vector3 normal = Vector3::yAxis();
	normal.copyToArray( mesh->vertices, p + 3 );
	p += numFloatsVertex;

	pos = transform.vector3XMatrix4( &v1 );
	pos.copyToArray( mesh->vertices, p );
	normal.copyToArray( mesh->vertices, p + 3 );
	p += numFloatsVertex;

	pos = transform.vector3XMatrix4( &v2 );
	pos.copyToArray( mesh->vertices, p );
	normal.copyToArray( mesh->vertices, p + 3 );

	return mesh;

}
