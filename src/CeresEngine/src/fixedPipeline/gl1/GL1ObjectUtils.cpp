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

#include "GL1ObjectUtils.h"
#include "math3d/importexport/STLMeshImporter.h"
#include "fixedPipeline/gl1/GL1Material.h"
#include "console/console.h"

using namespace ceres;

GL1ObjectUtils::GL1ObjectUtils() {

}

GL1ObjectUtils::~GL1ObjectUtils() {

}

GL1Mesh *GL1ObjectUtils::createObject(
	std::string modelPath,
	GL1Material *material,
	float radius,
	Vector3 position,
	std::string &error,
	GL1Mesh *object,
	float scale,
	bool smoothNormals ) {

	

	STLMeshImporter stlImporter;
	
	Mesh *mesh = loadSTLMESH( modelPath.c_str(), scale );
	if ( ! mesh ) {
		error = "Could not load mesh " + modelPath;
		return NULL;
	}

	if ( smoothNormals ) {
		Mesh *mergedMesh = mesh->mergeVertices();
		//delete mesh;
		mesh = mergedMesh;
		mesh->computeVertexNormals();
	}

	// Create the drawable object mesh

	if ( ! object ) object = new GL1Mesh();
	object->init( mesh, material );

	// Position

	object->pose = new Pose();
	object->pose->position.copy( &position );
	object->pose->updateMatrix();

	object->radius = radius;

	return object;

}

GL1Mesh *GL1ObjectUtils::createPNCObject(
	std::string modelPath,
	GL1Material *material,
	float radius,
	Vector3 position,
	std::string &error,
	GL1Mesh *object,
	float scale,
	bool smoothNormals ) {

	MeshImporter meshImporter;
	Mesh *mesh = meshImporter.loadMesh( modelPath.c_str(), scale );
	if ( ! mesh ) {
		error = "Could not load mesh " + modelPath;
		return NULL;
	}

	if ( smoothNormals ) {
		Mesh *mergedMesh = mesh->mergeVertices();
		delete mesh;
		mesh = mergedMesh;
		mesh->computeVertexNormals();
	}

	// Create the drawable object mesh

	if ( ! object ) object = new GL1Mesh();
	object->init( mesh, material );

	// Position

	object->pose = new Pose();
	object->pose->position.copy( &position );
	object->pose->updateMatrix();

	object->radius = radius;

	return object;

}
