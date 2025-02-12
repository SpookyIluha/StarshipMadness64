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

#include <stdexcept>

#include "ObjectLoader.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "math3d/importexport/MeshImporter.h"
#include "console/console.h"

using namespace ceres;

struct Y3DObject {

	bool thereIsPosition;
	bool thereIsNormal;
	bool thereIsColor;
	bool thereIsUV;

	uint64_t pncuFileSize;
	uint64_t indexFileSize;
	std::string texturePath;
	GraphicsColor diffuseColor;
	float metalness;
	float roughness;
	float opacity;
	bool transparent;

	uint8_t *vertices;
	uint8_t *indices;
};

ObjectLoader::ObjectLoader() {
}

ObjectLoader::~ObjectLoader() {
}

bool ObjectLoader::createY3DObject( Actuator *actuator, GL1Pipeline *pipeline, std::string modelPath, std::string texturePathPrefix, Vector3 position, float scale, std::string &error ) {

	actuator->data = NULL;
	actuator->object = NULL;
	actuator->physicsObject = NULL;

	// Load file into memory

	FILE *f = fopen( modelPath.c_str(), "r" );
	if ( ! f ) {
		error = std::string( "File not found: " ) + modelPath;
		return false;
	}

	MeshImporter meshImporter;
	size_t size = meshImporter.getFileSize( modelPath.c_str() );
	if ( size == 0 ) {
		fclose( f );
		error = std::string( "File size is 0: " ) + modelPath;
		return false;
	}

	uint8_t *fileContents = new uint8_t[ size ];
	if ( ! fileContents ) {
		fclose( f );
		error = std::string( "Not enough memory to load file (needed " ) + std::to_string( size ) + std::string( " bytes): " ) + modelPath;
		return false;
	}

	if ( fread( fileContents, size, 1, f ) != 1 ) {
		fclose( f );
		delete [] fileContents;
		error = std::string( "Error reading file: " ) + modelPath;
		return false;
	}

	fclose( f );

	// Parse file

	uint64_t cursor = 0;
	int32_t lineNumber = 1;

	std::string line = readLine( fileContents, cursor, size );
	lineNumber ++;
	if ( line != "Y3Dv1" ) {
		delete [] fileContents;
		error = std::string( "Unrecognized file format: " ) + modelPath;
		return false;
	}

	int32_t numObjects = readInteger( fileContents, cursor, size );
	lineNumber ++;

	Group *group = new Group();
	group->pose = new Pose();
	group->pose->position.copy( &position );
	group->pose->updateMatrix();

	if ( numObjects <= 0 ) {
		actuator->data = fileContents;
		actuator->object = group;
		return true;
	}

	std::vector<Y3DObject *> objects;

	for ( int32_t i = 0; i < numObjects; i ++ ) {

		Y3DObject *object = new Y3DObject();

		object->thereIsPosition = readLine( fileContents, cursor, size ) == "1";
		lineNumber ++;
		object->thereIsNormal = readLine( fileContents, cursor, size ) == "1";
		lineNumber ++;
		object->thereIsColor = readLine( fileContents, cursor, size ) == "1";
		lineNumber ++;
		object->thereIsUV = readLine( fileContents, cursor, size ) == "1";
		lineNumber ++;

		if ( ! object->thereIsPosition && ! object->thereIsNormal && ! object->thereIsColor && ! object->thereIsUV ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "There must be at least one active attribute. Line:" ) + std::to_string( lineNumber );
			return false;
		}

		object->pncuFileSize = readInteger( fileContents, cursor, size );
		lineNumber ++;
		if ( object->pncuFileSize <= 0 ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "Wrong vertex block size at line " ) + std::to_string( lineNumber );
			return false;
		}

		object->indexFileSize = readInteger( fileContents, cursor, size );
		lineNumber ++;
		if ( object->indexFileSize < 0 ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "Wrong indices block size at line " ) + std::to_string( lineNumber );
			return false;
		}

		line = readLine( fileContents, cursor, size );
		lineNumber ++;
		object->texturePath = line;

		object->diffuseColor.set(
			readFloat( fileContents, cursor, size ),
			readFloat( fileContents, cursor, size ),
			readFloat( fileContents, cursor, size )
		);
		lineNumber += 3;

		object->metalness = readFloat( fileContents, cursor, size );
		lineNumber ++;

		object->roughness = readFloat( fileContents, cursor, size );
		lineNumber ++;

		object->transparent = readInteger( fileContents, cursor, size ) == 1;
		lineNumber ++;

		object->opacity = readFloat( fileContents, cursor, size );
		lineNumber ++;

		objects.push_back( object );

	}

	for ( int32_t i = 0; i < 3; i ++ ) {
		if ( cursor % 4 == 0 ) break;
		readLine( fileContents, cursor, size );
	}

	uint64_t memoryCursor = cursor;

	for ( int32_t i = 0; i < numObjects; i ++ ) {

		Y3DObject *object = objects[ i ];

		if ( object->pncuFileSize + memoryCursor > size ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "Corrupted file in vertex block. pncuFileSize = " ) +
				std::to_string( object->pncuFileSize ) +
				std::string( ". memorycursor = " ) +
				std::to_string( memoryCursor ) +
				std::string( ". size = " ) +
				std::to_string( size ) +
				std::string( ": " ) + modelPath;
			return false;
		}

		object->vertices = fileContents + memoryCursor;
		memoryCursor += object->pncuFileSize;

		if ( object->indexFileSize + memoryCursor > size ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "Corrupted file in indices block: " ) + modelPath;
			return false;
		}

		object->indices = object->indexFileSize > 0 ? fileContents + memoryCursor : NULL;
		memoryCursor += object->indexFileSize;

	}

	// Create 3D objects

	for ( int32_t i = 0; i < numObjects; i ++ ) {

		Y3DObject *object = objects[ i ];

		size_t sizeFloats = object->pncuFileSize / 4;
		float *vertices = (float *)object->vertices;

		int32_t vertexSizeFloats = 0;
		if ( object->thereIsPosition ) vertexSizeFloats += 3;
		if ( object->thereIsNormal ) vertexSizeFloats += 3;
		if ( object->thereIsColor ) vertexSizeFloats += 3;
		if ( object->thereIsUV ) vertexSizeFloats += 2;

		if ( sizeFloats % vertexSizeFloats != 0 ) {
			for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];
			for ( int32_t j = 0; j < i; j ++ ) {
				group->objects[ i ]->mesh->vertices = NULL;
				group->objects[ i ]->mesh->indices = NULL;
				delete group->objects[ i ]->mesh;
			}
			delete [] fileContents;
			delete group->pose;
			delete group;
			error = std::string( "Corrupted file, number of bytes not round: " ) + modelPath;
			return false;
		}

		if ( object->thereIsPosition ) {
			for ( int64_t i = 0, n = sizeFloats; i < n; i += vertexSizeFloats ) {
				vertices[ i ] *= scale;
				vertices[ i + 1 ] *= scale;
				vertices[ i + 2 ] *= scale;
			}
		}

		Mesh *mesh = new Mesh();

		mesh->primitive = Mesh::MESH_TRIANGLES;
		mesh->numVertices = floor( sizeFloats / vertexSizeFloats );
		mesh->vertices = (float *)object->vertices;
		mesh->numIndices = floor( object->indexFileSize / sizeof( MeshIndexInt ) );
		mesh->indices = (MeshIndexInt *)object->indices;

		std::vector<Attribute> attributes;
		if ( object->thereIsPosition ) attributes.push_back( Attribute( "position", 3, 4, 0 ) );
		if ( object->thereIsNormal ) attributes.push_back( Attribute( "normal", 3, 4, 0 ) );
		if ( object->thereIsColor ) attributes.push_back( Attribute( "color", 3, 4, 0 ) );
		if ( object->thereIsUV ) attributes.push_back( Attribute( "uv", 2, 4, 0 ) );
		mesh->attributes = attributes;
		Mesh::computeAttributeOffsets( &mesh->attributes );

		GL1Material *material = new GL1Material();
		material->ambient.set( 0.0, 0.0, 0.0 );
		material->emission.set( 0.0, 0.0, 0.0 );
		material->diffuse.set( object->diffuseColor.r, object->diffuseColor.g, object->diffuseColor.b );
		material->specular.set( 0.0, 0.0, 0.0 );
		material->specularExponent = 0.0;
		material->illuminated = true;
		material->doubleSided = false;
		material->flatShading = false;
		material->transparent = object->transparent;
		material->opacity = object->opacity;
		material->depthTest = true;
		material->specularExponent = 0.0;
		//material->specularExponent = object->roughness;

		if ( object->texturePath.length() > 0 ) {

			material->texture = pipeline->loadTexture( texturePathPrefix + object->texturePath, error );

			if ( ! material->texture ) {

				for ( int32_t j = 0; j < i; j ++ ) delete objects[ j ];

				for ( int32_t j = 0; j < i; j ++ ) {
					group->objects[ j ]->mesh->vertices = NULL;
					group->objects[ j ]->mesh->indices = NULL;
					delete group->objects[ j ]->mesh;
				}

				delete [] fileContents;

				delete group->pose;

				//delete group;

				error = std::string( "Could not load texture: " ) + object->texturePath;
				return false;
			}
			//material->texture->repeat = true;
		}

		GL1Mesh *gl1Mesh = new GL1Mesh();
		gl1Mesh->init( mesh, material );

		gl1Mesh->pose = new Pose();
		gl1Mesh->pose->position.copy( &position );
		gl1Mesh->pose->updateMatrix();

		group->objects.push_back( gl1Mesh );

	}

	group->radius = 10.0;

	actuator->data = fileContents;
	actuator->object = group;
	actuator->physicsObject = NULL;

	return true;

}

std::string ObjectLoader::readLine( uint8_t *buffer, uint64_t &cursor, uint64_t size ) {
	uint64_t cursor0 = cursor;
	while ( ( cursor < size ) && ( buffer[ cursor ] != '\n' ) && ( buffer[ cursor ] != '\r' ) ) cursor ++;
	if ( cursor >= size ) return std::string( "" );
	buffer[ cursor ++ ] = 0;
	std::string line = (char *) ( buffer + cursor0 );
	return line;
}

int32_t ObjectLoader::readInteger( uint8_t *buffer, uint64_t &cursor, uint64_t size ) {
	std::string line = readLine( buffer, cursor, size );
	int32_t value = 0;
	try {
		value = std::stoi( line );
	} catch ( std::exception e ) {
		value = 0;
	}
	return value;
}

float ObjectLoader::readFloat( uint8_t *buffer, uint64_t &cursor, uint64_t size ) {
	std::string line = readLine( buffer, cursor, size );
	float value = 0.0;
	try {
		value = std::stof( line );
	} catch ( std::exception e ) {
		value = 0;
	}
	return value;
}
