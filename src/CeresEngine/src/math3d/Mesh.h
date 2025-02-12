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

#include <inttypes.h>
#include <string>
#include <vector>

#include "math3d/Vector3.h"
#include "math3d/IntersectionPoint.h"

namespace ceres {

class Attribute {

public:

	Attribute( const char *name, int numValues, int valueSizeBytes, int32_t glType );
	virtual ~Attribute();

	const char *name;
	int numValues;
	int valueSizeBytes;
	int32_t type;

	int32_t offset;
	int32_t offsetBytes;

	int32_t currentVertexCount;

	// Application dependent
	const char *units;

};

typedef uint32_t MeshIndexInt;

class Mesh {

public:

	Mesh();
	virtual ~Mesh();

	//typedef enum { MESH_POINTS = 0, MESH_LINES, MESH_TRIANGLES } Primitive;

	static constexpr int32_t MESH_POINTS = 0;
	static constexpr int32_t MESH_LINES = 1;
	static constexpr int32_t MESH_TRIANGLES = 2;

	bool isIndexed() { return numIndices > 0; }
	int32_t getNumVerticesPerPrimitive();
	int32_t getNumPrimitives();

	static int32_t computeAttributeSizeBytes( std::vector<Attribute> attributes );
	static int32_t computeAttributeSizeFloats( std::vector<Attribute> attributes );
	static int32_t  computeAttributeOffsets( std::vector<Attribute> *attributes );

	std::string getPrimitiveString();
	Attribute *getAttribute( std::string name );
	static Attribute *getAttribute( std::string name, std::vector<Attribute> &attributes );

	void computeVertexNormals();

	Mesh *mergeMesh( Mesh *otherMesh );
	Mesh *mergeVertices( scalar tolerance = 0.0001 );
	Mesh *toNonIndexedMesh();

	void center();

	//Primitive primitive;
	int32_t primitive;

	int32_t numVertices;
	float *vertices;

	int32_t numIndices;
	MeshIndexInt *indices;

	std::vector<Attribute> attributes;

private:

	const std::string primitiveTypesStrings[ 3 ] = { "points", "line segments", "triangles" };

};

}
