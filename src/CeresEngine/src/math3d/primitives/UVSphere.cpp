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

#include "UVSphere.h"
#include "math3d/Vector2.h"

using namespace ceres;

UVSphere::UVSphere() {
}


UVSphere::~UVSphere() {
}

Mesh * UVSphere::createUVSphere( scalar radius, Vector3 center, int32_t numSubdivisionsLat, int32_t numSubdivisionsLon, std::vector<Attribute> attributes ) {

	if ( radius < smallDelta ) return NULL;
	if ( numSubdivisionsLon < 3 || numSubdivisionsLat < 2 ) return NULL;

	Mesh::computeAttributeOffsets( &attributes );

	int32_t numTris = numSubdivisionsLat * numSubdivisionsLon * 2;
	int32_t numVertices = numSubdivisionsLon * ( numSubdivisionsLat  - 1 ) + 2;
	int32_t numFloatsVertex = Mesh::computeAttributeSizeFloats( attributes );

	Mesh *mesh = new Mesh();
	mesh->numVertices = numVertices;
	mesh->numIndices = numTris * 3;
	mesh->vertices = new float[ mesh->numVertices * numFloatsVertex ];
	mesh->indices = new MeshIndexInt[ mesh->numIndices ];
	mesh->attributes = attributes;

	Attribute *positionAttribute = mesh->getAttribute( "position" );
	if ( ! positionAttribute ) return NULL;
	int32_t positionOffset = positionAttribute->offset;

	Attribute *normalAttribute = mesh->getAttribute( "normal" );
	int32_t normalOffset = 0;
	if ( normalAttribute ) normalOffset = normalAttribute->offset;

	Attribute *uvAttribute = mesh->getAttribute( "uv" );
	int32_t uvOffset = 0;
	if ( uvAttribute ) uvOffset = uvAttribute->offset;

	// Pole vertices

	Vector3 northPole( 0.0, radius, 0.0 );
	northPole.inc( &center );
	Vector3 northPoleNormal( 0.0, 1.0, 0.0 );
	Vector2 northPoleUV( 0.5, 0.0 );

	Vector3 southPole( 0.0, - radius, 0.0 );
	southPole.inc( &center );
	Vector3 southPoleNormal( 0.0, - 1.0, 0.0 );
	Vector2 southPoleUV( 0.5, 1.0 );

	northPole.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 2 ) + positionOffset );
	southPole.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 1 ) + positionOffset );
	if ( normalAttribute ) {
		northPoleNormal.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 2 ) + normalOffset );
		southPoleNormal.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 1 ) + normalOffset );
	}
	if ( uvAttribute ) {
		northPoleUV.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 2 ) + uvOffset );
		southPoleUV.copyToArray( mesh->vertices, numFloatsVertex * ( numVertices - 1 ) + uvOffset );
	}

	// Intermediate vertices

	scalar angLon = 0.0;
	scalar dAngLon = 2.0 * pi / numSubdivisionsLon;
	for ( int32_t iLon = 0; iLon < numSubdivisionsLon; iLon++ ) {
		scalar dAngLat = pi / numSubdivisionsLat;
		scalar angLat = dAngLat;
		for ( int32_t iLat = 0; iLat < numSubdivisionsLat - 1; iLat++ ) {

			Vector3 vertex;
			Vector3 normal;
			scalar rX = sin( angLat );
			normal.set( rX * cos( angLon ), cos( angLat ), rX * sin( angLon ) );
			vertex = normal;
			vertex.multiplyScalar( radius )->inc( &center );

			int32_t vertexIndex = iLon * ( numSubdivisionsLat -1 ) + iLat;

			vertex.copyToArray( mesh->vertices, numFloatsVertex * vertexIndex + positionOffset );
			if ( normalAttribute ) normal.copyToArray( mesh->vertices, numFloatsVertex * vertexIndex + normalOffset );
			if ( uvAttribute ) {

				Vector2 uv;
				uv.set(
					iLon / ( numSubdivisionsLon - 1.0 ),
					( iLat + 1.0 ) / numSubdivisionsLat
				);
				uv.copyToArray( mesh->vertices, numFloatsVertex * vertexIndex + uvOffset );

			}

			angLat += dAngLat;
		}
		angLon += dAngLon;
	}

	// Create triangle indices
	int32_t vertexIndex = 0;
	for ( int32_t iLon = 1; iLon < numSubdivisionsLon; iLon++ ) {

		// North triangle

		mesh->indices[ vertexIndex ++ ] = numVertices - 2;
		mesh->indices[ vertexIndex ++ ] = iLon * ( numSubdivisionsLat - 1 );
		mesh->indices[ vertexIndex ++ ] = ( iLon - 1 ) * ( numSubdivisionsLat - 1 );

		// South triangle

		mesh->indices[ vertexIndex ++ ] = numVertices - 1;
		mesh->indices[ vertexIndex ++ ] = ( iLon - 1 ) * ( numSubdivisionsLat - 1 ) + numSubdivisionsLat - 2;
		mesh->indices[ vertexIndex ++ ] = iLon * ( numSubdivisionsLat - 1 ) + numSubdivisionsLat - 2;


		// Rest of triangles
		for ( int32_t iLat = 1; iLat < numSubdivisionsLat - 1; iLat++ ) {

			mesh->indices[ vertexIndex ++ ] = ( iLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat - 1;
			mesh->indices[ vertexIndex ++ ] = iLon * ( numSubdivisionsLat -1 ) + iLat;
			mesh->indices[ vertexIndex ++ ] = ( iLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat;

			mesh->indices[ vertexIndex ++ ] = ( iLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat - 1;
			mesh->indices[ vertexIndex ++ ] = iLon * ( numSubdivisionsLat - 1 ) + iLat - 1;
			mesh->indices[ vertexIndex ++ ] = iLon * ( numSubdivisionsLat -1 ) + iLat;

		}

	}

	// Create triangles from last strip

	// North triangle

	mesh->indices[ vertexIndex ++ ] = numVertices - 2;
	mesh->indices[ vertexIndex ++ ] = 0 * ( numSubdivisionsLat - 1 );
	mesh->indices[ vertexIndex ++ ] = ( numSubdivisionsLon - 1 ) * ( numSubdivisionsLat - 1 );

	// South triangle

	mesh->indices[ vertexIndex ++ ] = numVertices - 1;
	mesh->indices[ vertexIndex ++ ] = ( numSubdivisionsLon - 1 ) * ( numSubdivisionsLat - 1 ) + numSubdivisionsLat - 2;
	mesh->indices[ vertexIndex ++ ] = 0 * ( numSubdivisionsLat - 1 ) + numSubdivisionsLat- 2;


	// Rest of strip
	for ( int iLat = 1; iLat < numSubdivisionsLat - 1; iLat++ ) {

		mesh->indices[ vertexIndex ++ ] = ( numSubdivisionsLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat - 1;
		mesh->indices[ vertexIndex ++ ] = 0 * ( numSubdivisionsLat - 1 ) + iLat;
		mesh->indices[ vertexIndex ++ ] = ( numSubdivisionsLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat;

		mesh->indices[ vertexIndex ++ ] = ( numSubdivisionsLon - 1 ) * ( numSubdivisionsLat - 1 ) + iLat - 1;
		mesh->indices[ vertexIndex ++ ] = 0 * ( numSubdivisionsLat - 1 ) + iLat - 1;
		mesh->indices[ vertexIndex ++ ] = 0 * ( numSubdivisionsLat - 1 ) + iLat;

	}

	return mesh;

}
