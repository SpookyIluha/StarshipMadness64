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

#include "STLMeshImporter.h"

#include <stdio.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <libdragon.h>
#include "utils/utils.h"


using namespace ceres;

STLMeshImporter::STLMeshImporter() {}

STLMeshImporter::~STLMeshImporter() {}

ceres::Mesh * loadSTLMESH(const char *path, float scale ) {
	struct stat st;
    if ( stat( path, &st ) != 0 ) return NULL;
	off_t length = st.st_size;

	if ( length < 84 ) return NULL;

	FILE * f = fopen( path, "rb" );

	if ( ! f ) return NULL;

	if ( fseek( f, 80, SEEK_SET ) != 0 ) {
		fclose( f );
		return NULL;
	}

	uint32_t numTriangles = 0;
	off_t read = fread( &numTriangles, 1, 4, f );
	numTriangles = byteswap_uint32(numTriangles);
	if ( read != 4 ) {
		fclose( f );
		return NULL;
	}

	if ( numTriangles == 0 ) {
		fclose( f );
		return NULL;
	}

	unsigned int stlBufferLength = numTriangles * ( 4 * 3 * 4 + 2 );

	if ( stlBufferLength + 84 != ((unsigned int)length) ) {
		fclose( f );
		return NULL;
	}

	unsigned int bufferLength = numTriangles * ( 3 * 2 * 3 /*+ 2*/ );

	float *buf = new float[ bufferLength ];

	if ( ! buf ) {
		fclose( f );
		return NULL;
	}

	int vectorSize = 3 * sizeof( float );
	float normal[ 3 ];
	float pos[ 3 ];

	float *p = buf;
	for ( unsigned int i = 0; i < numTriangles; i ++ ) {

		if ( fread( normal, 1, vectorSize, f ) < ((unsigned long)vectorSize) ) {
			fclose( f );
			delete [] buf;
			return NULL;
		}
		for ( int n = 0; n < 3; n ++ ) normal[n] = byteswap_float(normal[n]);

		for ( int v = 0; v < 3; v ++ ) {

			if ( fread( pos, 1, vectorSize, f ) < ((unsigned long)vectorSize) ) {
				fclose( f );
				delete [] buf;
				return NULL;
			}
			for ( int np = 0; np < 3; np ++ ) pos[np] = byteswap_float(pos[np]);

			*p ++ = pos[ 0 ] * scale;
			*p ++ = pos[ 2 ] * scale;
			*p ++ = - pos[ 1 ] * scale;

			*p ++ = normal[ 0 ];
			*p ++ = normal[ 2 ];
			*p ++ = - normal[ 1 ];

		}

		int sizeExtra = 2;
		unsigned char extra[ sizeExtra ];
		if ( fread( extra, 1, sizeExtra, f ) < ((unsigned long)sizeExtra) ) {
			fclose( f );
			delete [] buf;
			return NULL;
		}

		// use extra
		// p += 2;

	}

	fclose( f );


	Mesh *mesh = new Mesh();
	mesh->numVertices = numTriangles * 3;
	mesh->numIndices = 0;
	mesh->vertices = buf;
	mesh->indices = NULL;
	mesh->attributes.push_back( Attribute( "position", 3, 4, 0 ) );
	mesh->attributes.push_back( Attribute( "normal", 3, 4, 0 ) );
	Mesh::computeAttributeOffsets( &mesh->attributes );

	return mesh;

}

Mesh* STLMeshImporter::loadMesh( const char *path, float scale ){
	return loadMesh( path, scale );
}