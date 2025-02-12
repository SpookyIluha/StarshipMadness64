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

#include "STLMeshExporter.h"

#include <math.h>

#include "math3d/Vector3.h"

#include "console/console.h"

using namespace ceres;

bool STLMeshExporter::exportSTL( Mesh * mesh, std::string path, float scale, std::string &error ) {

	uint32_t numTriangles = mesh->getNumPrimitives();

	FILE *f = fopen( path.c_str(), "w" );

	if ( ! f ) {

		error = std::string( "Could not open output mesh file for writing: " ) + path;
		return false;

	}

	if ( ! writeSTLHeader( f, path, error ) ) return false;

	if ( ! writeUInt32( f, path, numTriangles, error ) ) return false;

	//Vector3 *vertices = (Vector3 *)mesh->vertices;
	float *vertices = mesh->vertices;
	MeshIndexInt *triangleIndices = mesh->indices;
	int32_t attributeStride = mesh->attributes.size();

	for ( int32_t i = 0, il = mesh->numIndices > 0 ? mesh->numIndices : mesh->numVertices; i < il; i += 3 ) {

		uint32_t a, b, c;

		if ( mesh->numIndices > 0 ) {

			a = triangleIndices[ i ];
			b = triangleIndices[ i + 1 ];
			c = triangleIndices[ i + 2 ];

		}
		else {

			a = i;
			b = i + 1;
			c = i + 2;

		}

		Vector3 p1;
		p1.copyFromArray( vertices, 3 * attributeStride * a );
		Vector3 p2;
		p2.copyFromArray( vertices, 3 * attributeStride * b );
		Vector3 p3;
		p3.copyFromArray( vertices,  3 * attributeStride * c );

		p1.multiplyScalar( scale );
		p2.multiplyScalar( scale );
		p3.multiplyScalar( scale );

		float floatTemp;
		floatTemp = p1.y; p1.y = - p1.z; p1.z = floatTemp;
		floatTemp = p2.y; p2.y = - p2.z; p2.z = floatTemp;
		floatTemp = p3.y; p3.y = - p3.z; p3.z = floatTemp;

		Vector3 v1, v2, normal;
		v1.sub( &p2, &p1 );
		v2.sub( &p3, &p1 );
		normal.crossProduct( &v1, &v2 )->normalize();

		// Write normal
		if ( ! writeFloat( f, path, normal.x, error ) ) return false;
		if ( ! writeFloat( f, path, normal.y, error ) ) return false;
		if ( ! writeFloat( f, path, normal.z, error ) ) return false;

		// Write vertices
		if ( ! writeFloat( f, path, p1.x, error ) ) return false;
		if ( ! writeFloat( f, path, p1.y, error ) ) return false;
		if ( ! writeFloat( f, path, p1.z, error ) ) return false;

		if ( ! writeFloat( f, path, p2.x, error ) ) return false;
		if ( ! writeFloat( f, path, p2.y, error ) ) return false;
		if ( ! writeFloat( f, path, p2.z, error ) ) return false;

		if ( ! writeFloat( f, path, p3.x, error ) ) return false;
		if ( ! writeFloat( f, path, p3.y, error ) ) return false;
		if ( ! writeFloat( f, path, p3.z, error ) ) return false;

		// Write byte count (not used)
		if ( ! writeUInt16( f, path, 0, error ) ) return false;

	}

	fclose( f );

	return true;

}

bool STLMeshExporter::writeSTLHeader( FILE *f, std::string path, std::string &error ) {

	uint8_t header[ 80 ];
	for ( int i = 0; i < 80; i ++ ) header[ i ] = 0;

	if ( fwrite( header, 80, 1, f ) != 1 ) {

		error = std::string( "Could not write output mesh file: " ) + path;
		fclose( f );
		return false;

	}

	return true;

}

bool STLMeshExporter::writeUInt32( FILE *f, std::string path, uint32_t value, std::string &error ) {

	if ( fwrite( &value, 4, 1, f ) != 1 ) {

		error = std::string( "Could not write output mesh file: " ) + path;
		fclose( f );
		return false;

	}

	return true;

}

bool STLMeshExporter::writeUInt16( FILE *f, std::string path, uint16_t value, std::string &error ) {

	if ( fwrite( &value, 2, 1, f ) != 1 ) {

		error = std::string( "Could not write output mesh file: " ) + path;
		fclose( f );
		return false;

	}

	return true;

}

bool STLMeshExporter::writeFloat( FILE *f, std::string path, float value, std::string &error ) {

	if ( fwrite( &value, 4, 1, f ) != 1 ) {

		error = std::string( "Could not write output mesh file: " ) + path;
		fclose( f );
		return false;

	}

	return true;

}
