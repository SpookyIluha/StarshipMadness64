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

#include "Rectangle.h"
#include "math3d/Vector2.h"

using namespace ceres;

Rectangle::Rectangle() {
}


Rectangle::~Rectangle() {
}

Mesh *Rectangle::createRectangle( scalar width, scalar depth, int32_t numSubdivisionsX, int32_t numSubdivisionsZ, bool centered, std::vector<Attribute> attributes, Matrix4 transform ) {

	if ( width < smallDelta || depth < smallDelta ) return NULL;
	if ( numSubdivisionsX < 1 || numSubdivisionsZ < 1 ) return NULL;

	int32_t numFloatsVertex = Mesh::computeAttributeSizeFloats( attributes );

	Mesh *mesh = new Mesh();
	mesh->numVertices = ( numSubdivisionsX + 1 ) * ( numSubdivisionsZ + 1 );
	mesh->numIndices = numSubdivisionsX * numSubdivisionsZ * 2 * 3;
	mesh->vertices = new float[ mesh->numVertices * numFloatsVertex ];
	mesh->indices = new MeshIndexInt[ mesh->numIndices ];
	mesh->attributes = attributes;
	mesh->primitive = Mesh::MESH_TRIANGLES;

	Vector3 offset = centered ? Vector3( - width * 0.5, 0.0, depth * 0.5 ) : Vector3::origin();

	Attribute *normalAttr = Mesh::getAttribute( "normal", attributes );
	Attribute *uvAttr = Mesh::getAttribute( "uv", attributes );

	int32_t p = 0;
	for ( int32_t j = 0; j < numSubdivisionsZ + 1; j ++ ) {

		for ( int32_t i = 0; i < numSubdivisionsX + 1; i ++ ) {

			Vector3 pos = Vector3(
				i * ( width / numSubdivisionsX ),
				0.0,
				- j * ( depth / numSubdivisionsZ )
			);
			pos.inc( &offset );
			pos = transform.vector3XMatrix4( &pos );
			pos.copyToArray( mesh->vertices, p );

			if ( normalAttr ) {
				Vector3 normal = Vector3::yAxis();
				normal.copyToArray( mesh->vertices, p + normalAttr->offset );
			}

			if ( uvAttr ) {
				Vector2 uv(
					((float) i ) / ((float) ( numSubdivisionsX + 1 ) ),
					((float) j ) / ((float) ( numSubdivisionsZ + 1 ) ));
				uv.copyToArray( mesh->vertices, p + uvAttr->offset );
			}

			p += numFloatsVertex;

		}

	}

	p = 0;
	for ( int32_t j = 0; j < numSubdivisionsZ; j ++ ) {

		for ( int32_t i = 0; i < numSubdivisionsX; i ++ ) {

			mesh->indices[ p ++ ] = i + j * ( numSubdivisionsX + 1 );
			mesh->indices[ p ++ ] = ( i + 1 ) + j * ( numSubdivisionsX + 1 );
			mesh->indices[ p ++ ] = ( i + 1 ) + ( j + 1 ) * ( numSubdivisionsX + 1 );

			mesh->indices[ p ++ ] = i + j * ( numSubdivisionsX + 1 );
			mesh->indices[ p ++ ] = ( i + 1 ) + ( j + 1 ) * ( numSubdivisionsX + 1 );
			mesh->indices[ p ++ ] = i + ( j + 1 ) * ( numSubdivisionsX + 1 );

		}

	}

	return mesh;

}
