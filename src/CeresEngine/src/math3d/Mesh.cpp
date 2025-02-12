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

#include "Mesh.h"

#include "math3d/IntersectionPoint.h"
#include "console/console.h"

#include <cstring>
#include <math.h>
#include <algorithm>

using namespace ceres;

Attribute::Attribute( const char *name, int numValues, int valueSizeBytes, int32_t type ) {

	this->name = name;
	this->numValues = numValues;
	this->valueSizeBytes = valueSizeBytes;
	this->type = type;
	this->offset = 0;
	units = NULL;

}

Attribute::~Attribute() {}

Mesh::Mesh():
	primitive( MESH_TRIANGLES ),
	numVertices( 0 ),
	vertices( NULL ),
	numIndices( 0 ),
	indices( NULL ) {
}

Mesh::~Mesh() {

	delete [] vertices;
	delete [] indices;

}

int32_t Mesh::getNumVerticesPerPrimitive() {

	switch ( primitive ) {

		case MESH_POINTS: return 1;
		case MESH_LINES: return 2;
		case MESH_TRIANGLES: return 3;
		default: return 1;

	}

}

int32_t Mesh::getNumPrimitives() {

	if ( isIndexed() ) return numIndices / getNumVerticesPerPrimitive();
	else return numVertices / getNumVerticesPerPrimitive();

}

int32_t Mesh::computeAttributeSizeBytes( std::vector<Attribute> attributes ) {

	int32_t vertexSizeBytes = 0;
	for ( uint32_t i = 0, il = attributes.size(); i < il;  i ++ ) {

		Attribute *attr = &attributes[ i ];

		vertexSizeBytes += attr->numValues * attr->valueSizeBytes;

	}

	return vertexSizeBytes;

}

int32_t Mesh::computeAttributeSizeFloats( std::vector<Attribute> attributes ) {

	// All attributes assumed to have only floats

	int32_t vertexSizeFloats = 0;
	for ( uint32_t i = 0, il = attributes.size(); i < il;  i ++ ) {

		Attribute *attr = &attributes[ i ];

		if ( attr->valueSizeBytes != 4 ) return 0;

		vertexSizeFloats += attr->numValues;

	}

	return vertexSizeFloats;

}

int32_t Mesh::computeAttributeOffsets( std::vector<Attribute> *attributes ) {

	// Returns size in bytes of all attributes

	int32_t attribOffset = 0;
	for ( int32_t i = 0, il = attributes->size(); i < il;  i ++ ) {

		Attribute *attr = &( (*attributes)[ i ] );
		attr->offsetBytes = attribOffset;
		attr->offset = attribOffset / 4;
		attribOffset += attr->numValues * attr->valueSizeBytes;

	}

	return attribOffset;

}

std::string Mesh::getPrimitiveString() {

	return primitiveTypesStrings[ primitive ];

}

Attribute *Mesh::getAttribute( std::string name ) {

	for ( int32_t i = 0, n = attributes.size(); i < n; i ++ ) {

		if ( attributes[ i ].name == name ) return &attributes[ i ];

	}

	return NULL;

}

Attribute *Mesh::getAttribute( std::string name, std::vector<Attribute> &attributes ) {

	for ( int32_t i = 0, n = attributes.size(); i < n; i ++ ) {

		if ( attributes[ i ].name == name ) return &attributes[ i ];

	}

	return NULL;

}

Mesh *Mesh::mergeMesh( Mesh *otherMesh ) {

	// otherMesh attributes are assumed to be equal to this mesh attributes.

	if ( numVertices == 0 ) return NULL;

	std::vector<float *> newVerticesList;
	std::vector<MeshIndexInt> newIndicesList;

	MeshIndexInt numFloatsVertex = computeAttributeSizeFloats( attributes );

	// Add this mesh
	if ( isIndexed() ) {

		for ( int32_t i = 0; i < numIndices; i ++ ) newIndicesList.push_back( indices[ i ] );

		for ( int32_t i = 0; i < numVertices; i ++ ) {

			float *newVertex = new float[ numFloatsVertex ];
			memcpy( newVertex, &vertices[ i * numFloatsVertex ], numFloatsVertex * 4 );
			newVerticesList.push_back( newVertex );

		}

	} else {

		float *p = vertices;
		for ( int32_t i = 0; i < numVertices; i ++ ) {

			MeshIndexInt index = newVerticesList.size();
			float *newVertex = new float[ numFloatsVertex ];
			memcpy( newVertex, p, numFloatsVertex * 4 );
			newVerticesList.push_back( newVertex );
			newIndicesList.push_back( index );
			p += numFloatsVertex;

		}

	}

	// Add otherMesh
	if ( otherMesh->isIndexed() ) {

		MeshIndexInt i0 = newVerticesList.size();

		for ( int32_t i = 0; i < otherMesh->numIndices; i ++ ) newIndicesList.push_back( otherMesh->indices[ i ] + i0 );

		for ( int32_t i = 0; i < otherMesh->numVertices; i ++ ) {

			float *newVertex = new float[ numFloatsVertex ];
			memcpy( newVertex, &otherMesh->vertices[ i * numFloatsVertex ], numFloatsVertex * 4 );
			newVerticesList.push_back( newVertex );

		}

	} else {

		float *p = otherMesh->vertices;
		for ( int32_t i = 0; i < otherMesh->numVertices; i ++ ) {

			MeshIndexInt index = newVerticesList.size();
			float *newVertex = new float[ numFloatsVertex ];
			memcpy( newVertex, p, numFloatsVertex * 4 );
			newVerticesList.push_back( newVertex );
			newIndicesList.push_back( index );
			p += numFloatsVertex;

		}

	}

	bool err = newVerticesList.size() >= 1024LL * 64LL;

	MeshIndexInt numNewVertices = newVerticesList.size();
	MeshIndexInt numNewIndices = newIndicesList.size();

	float *newVertices = NULL;
	if ( ! err ) {

		newVertices = new float[ numNewVertices * numFloatsVertex ];
		for ( MeshIndexInt i = 0; i < numNewVertices; i ++ ) {

			memcpy( &newVertices[ i * numFloatsVertex ], newVerticesList[ i ], numFloatsVertex * 4 );

		}

	}

	for ( int32_t i = 0, in = newVerticesList.size(); i < in; i ++ ) delete newVerticesList[ i ];

	if ( err ) return NULL;

	MeshIndexInt *newIndices = new MeshIndexInt[ numNewIndices ];
	MeshIndexInt *pi1 = newIndices;
	for ( uint32_t i = 0; i < numNewIndices; i ++ ) *pi1 ++ = newIndicesList[ i ];

	Mesh *newMesh = new Mesh();
	newMesh->numVertices = numNewVertices;
	newMesh->numIndices = numNewIndices;
	newMesh->vertices = newVertices;
	newMesh->indices = newIndices;
	newMesh->attributes = attributes;

	return newMesh;

}

Mesh *Mesh::mergeVertices( scalar tolerance ) {

	if ( numVertices == 0 ) return NULL;

	std::vector<float *> newVerticesList;
	std::vector<MeshIndexInt> newIndicesList;

	MeshIndexInt numFloatsVertex = computeAttributeSizeFloats( attributes );

	Vector3 pos;

	if ( isIndexed() ) {

		for ( int32_t i = 0; i < numIndices; i ++ ) {

			pos.copyFromArray( &vertices[ indices[ i ] * numFloatsVertex ] );

			MeshIndexInt index = 0;
			bool foundVertex = false;
			for ( MeshIndexInt j = 0, jn = newVerticesList.size(); j < jn; j ++ ) {

				Vector3 pos2;
				pos2.copyFromArray( newVerticesList[ j ] );
				scalar d = pos.distanceTo( &pos2 );
				if ( d < tolerance ) {

					index = j;
					foundVertex = true;
					break;

				}

			}

			if ( ! foundVertex ) {

				index = newVerticesList.size();

				float *newVertex = new float[ numFloatsVertex ];

				memcpy( newVertex, &vertices[ indices[ i ] * numFloatsVertex ], numFloatsVertex * 4 );

				newVerticesList.push_back( newVertex );

			}

			newIndicesList.push_back( index );

		}

	} else {

		float *p = vertices;
		for ( int32_t i = 0; i < numVertices; i ++ ) {

			pos.copyFromArray( p );

			MeshIndexInt index = 0;
			bool foundVertex = false;
			for ( MeshIndexInt j = 0, jn = newVerticesList.size(); j < jn; j ++ ) {

				Vector3 pos2;
				pos2.copyFromArray( newVerticesList[ j ] );
				scalar d = pos.distanceTo( &pos2 );
				if ( d < tolerance ) {

					index = j;
					foundVertex = true;
					break;

				}

			}

			if ( ! foundVertex ) {

				index = newVerticesList.size();

				float *newVertex = new float[ numFloatsVertex ];

				memcpy( newVertex, p, numFloatsVertex * 4 );

				newVerticesList.push_back( newVertex );

			}

			newIndicesList.push_back( index );

			p += numFloatsVertex;

		}

	}

	bool err = newVerticesList.size() >= 1024LL * 64LL;

	MeshIndexInt numNewVertices = newVerticesList.size();
	MeshIndexInt numNewIndices = newIndicesList.size();

	float *newVertices = NULL;
	if ( ! err ) {

		newVertices = new float[ numNewVertices * numFloatsVertex ];
		for ( MeshIndexInt i = 0; i < numNewVertices; i ++ ) {

			memcpy( &newVertices[ i * numFloatsVertex ], newVerticesList[ i ], numFloatsVertex * 4 );

		}

	}

	for ( int32_t i = 0, in = newVerticesList.size(); i < in; i ++ ) delete newVerticesList[ i ];

	if ( err ) return NULL;

	MeshIndexInt *newIndices = new MeshIndexInt[ numNewIndices ];
	MeshIndexInt *pi = newIndices;
	for ( uint32_t i = 0; i < numNewIndices; i ++ ) *pi ++ = newIndicesList[ i ];

	Mesh *mesh = new Mesh();
	mesh->numVertices = numNewVertices;
	mesh->numIndices = numNewIndices;
	mesh->vertices = newVertices;
	mesh->indices = newIndices;
	mesh->attributes = attributes;
	mesh->primitive = primitive;

	return mesh;

}

Mesh *Mesh::toNonIndexedMesh() {

	if ( ! isIndexed() ) return NULL;
	if ( numVertices == 0 ) return NULL;
	if ( numIndices == 0 ) return NULL;

	int32_t numNewVertices = numIndices;
	int32_t numFloatsVertex = computeAttributeSizeFloats( attributes );
	int32_t numBytesVertex = numFloatsVertex * 4;
	float *newVertices = new float[ numNewVertices * numFloatsVertex ];

	for ( int32_t i = 0; i < numIndices; i ++ ) {
		memcpy( &newVertices[ i * numFloatsVertex ], &vertices[ indices[ i ] * numFloatsVertex ], numBytesVertex );
	}

	Mesh *mesh = new Mesh();
	mesh->numVertices = numNewVertices;
	mesh->numIndices = 0;
	mesh->vertices = newVertices;
	mesh->indices = NULL;
	mesh->attributes = attributes;
	mesh->primitive = primitive;

	return mesh;

}

void Mesh::computeVertexNormals() {

	if ( numVertices == 0 ) return;
	if ( primitive != MESH_TRIANGLES ) return;

	uint32_t numFloatsVertex = computeAttributeSizeFloats( attributes );
	Attribute *normalAttribute = getAttribute( std::string( "normal" ) );
	if ( ! normalAttribute ) return;
	int32_t normalOffset = normalAttribute->offset;

	Vector3 v0, v1, v2, v01, v02, normal, n0, n1, n2;

	if ( isIndexed() ) {

		float *newNormals = new float[ numVertices * 3 ];
		for ( int32_t i = 0, in = numVertices * 3; i < in; i ++ ) newNormals[ i ] = 0.0;

		for ( int32_t i = 0; i < numIndices; i += 3 ) {

			uint32_t iv0 = indices[ i ];
			uint32_t iv1 = indices[ i + 1 ];
			uint32_t iv2 = indices[ i + 2 ];

			v0.copyFromArray( &vertices[ iv0 * numFloatsVertex ] );
			v1.copyFromArray( &vertices[ iv1 * numFloatsVertex ] );
			v2.copyFromArray( &vertices[ iv2 * numFloatsVertex ] );

			v01.sub( &v1, &v0 );
			v02.sub( &v2, &v0 );
			normal.crossProduct( &v01, &v02 )->normalize();

			n0.copyFromArray( &newNormals[ iv0 * 3 ] );
			n1.copyFromArray( &newNormals[ iv1 * 3 ] );
			n2.copyFromArray( &newNormals[ iv2 * 3 ] );

			n0.inc( &normal );
			n1.inc( &normal );
			n2.inc( &normal );

			n0.copyToArray( &newNormals[ iv0 * 3] );
			n1.copyToArray( &newNormals[ iv1 * 3] );
			n2.copyToArray( &newNormals[ iv2 * 3] );

		}

		for ( int32_t i = 0, in = numVertices; i < in; i ++ ) {

			normal.copyFromArray( &newNormals[ i * 3 ] );
			normal.copyToArray( &vertices[ i * numFloatsVertex + normalOffset ] );

		}

		delete [] newNormals;

	} else {

		for ( int32_t i = 0; i < numVertices; i += 3 ) {

			v0.copyFromArray( &vertices[ i * numFloatsVertex ] );
			v1.copyFromArray( &vertices[ ( i + 1 ) * numFloatsVertex ] );
			v2.copyFromArray( &vertices[ ( i + 2 ) * numFloatsVertex ] );

			v01.sub( &v1, &v0 );
			v02.sub( &v2, &v0 );
			normal.crossProduct( &v01, &v02 )->normalize();

			v0.copyToArray( &vertices[ i * numFloatsVertex + normalOffset] );
			v1.copyToArray( &vertices[ ( i + 1 ) * numFloatsVertex + normalOffset ] );
			v2.copyToArray( &vertices[ ( i + 2 ) * numFloatsVertex + normalOffset ] );

		}

	}

}

void Mesh::center() {

	Vector3 center;

	uint32_t numFloatsVertex = computeAttributeSizeFloats( attributes );

	Vector3 minPos( 10000000000, 10000000000, 10000000000 );
	Vector3 maxPos( -10000000000, -10000000000, -10000000000 );
	for ( int32_t i = 0; i < numVertices; i ++ ) {

		Vector3 pos;
		pos.copyFromArray( &vertices[ i * numFloatsVertex ] );

		if ( pos.x < minPos.x ) minPos.x = pos.x;
		if ( pos.y < minPos.y ) minPos.y = pos.y;
		if ( pos.z < minPos.z ) minPos.z = pos.z;

		if ( pos.x > maxPos.x ) maxPos.x = pos.x;
		if ( pos.y > maxPos.y ) maxPos.y = pos.y;
		if ( pos.z > maxPos.z ) maxPos.z = pos.z;

	}

	center.add( &minPos, &maxPos );
	center.multiplyScalar( 0.5 );
	for ( int32_t i = 0; i < numVertices; i ++ ) {
		Vector3 pos;
		pos.copyFromArray( vertices, i * numFloatsVertex );
		pos.dec( &center );
		pos.copyToArray( vertices, i * numFloatsVertex );
	}

}
