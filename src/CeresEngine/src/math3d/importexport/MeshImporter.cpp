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

#include "MeshImporter.h"

#include "console/console.h"

#include <sys/stat.h>

using namespace ceres;

MeshImporter::MeshImporter() {}

MeshImporter::~MeshImporter() {}

Mesh *MeshImporter::loadMesh( const char *path, float scale ) {

	FILE *f = fopen( path, "r" );
	if ( ! f ) return NULL;


	size_t size = getFileSize( path );
	size_t sizeFloats = size / 4;
	float *buffer = new float[ sizeFloats ];
	if ( fread( buffer, sizeFloats * 4, 1, f ) != 1 ) {
		fclose( f );
		delete [] buffer;
		return NULL;
	}

	Vector3 minPos, maxPos;

	bool normal = sizeFloats % 8 == 0;

	int32_t inc = normal ? 5 : 3;
	int32_t p = 0;
	for ( int32_t i = 0, n = sizeFloats; i < n; i += inc + 3 ) {

		buffer[ p ++ ] *= scale;
		buffer[ p ++ ] *= scale;
		buffer[ p ++ ] *= scale;
		p += inc;

	}

	Mesh *mesh = new Mesh();

	int32_t numFloatsPerVertex = normal ? 8 : 5;

	mesh->primitive = Mesh::MESH_TRIANGLES;
	mesh->numVertices = floor( sizeFloats / numFloatsPerVertex );
	mesh->vertices = buffer;
	mesh->numIndices = 0;
	mesh->indices = NULL;

	std::vector<Attribute> attributes;
	attributes.push_back( Attribute( "position", 3, 4, 0 ) );
	if ( normal ) attributes.push_back( Attribute( "normal", 3, 4, 0 ) );
	attributes.push_back( Attribute( "uv", 2, 4, 0 ) );
	mesh->attributes = attributes;
	Mesh::computeAttributeOffsets( &mesh->attributes );

	fclose( f );

	return mesh;

}

size_t MeshImporter::getFileSize( const char *filename ) {
	struct stat st;
	if ( stat( filename, &st ) == 0 ) return st.st_size;
	else return 0;
}
