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

#include "GL1Mesh.h"

#include "console/console.h"

using namespace ceres;

GL1Mesh::GL1Mesh():
	attributeStrideBytes( 0 ) {
}

GL1Mesh::~GL1Mesh() {
	rspq_wait();
	if(this->mesh->block != nullptr)
		rspq_block_free(this->mesh->block);
	this->mesh->block = nullptr;
}

bool GL1Mesh::init( Mesh *mesh, FPMaterial *material ) {

	this->mesh = mesh;
	this->material = material;

	attributeStrideBytes = Mesh::computeAttributeOffsets( &mesh->attributes );
	currentVertexCount = mesh->isIndexed() ? mesh->numIndices : mesh->numVertices;

	normalAttribute = mesh->getAttribute( "normal" );
	uvAttribute = mesh->getAttribute( "uv" );

	this->mesh->block = NULL;

	return true;
}

void GL1Mesh::render( Camera *camera ) {

	if ( ! visible ) return;

	setupRender( camera );

	// Setup attributes arrays

	scalar *vertices = mesh->vertices;

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, attributeStrideBytes, vertices );

	if ( normalAttribute ) {
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, attributeStrideBytes, vertices + ( normalAttribute->offset ) );
	} else glDisableClientState( GL_NORMAL_ARRAY );


	if ( uvAttribute ) {
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, attributeStrideBytes, vertices + ( uvAttribute->offset ) );
	} else glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// Draw the primitives

	if(!this->mesh->block){
		rspq_block_begin();
		if ( mesh->isIndexed() ) glDrawElements( glPrimitives[ mesh->primitive ], currentVertexCount, GL_UNSIGNED_INT, mesh->indices );
		else glDrawArrays( glPrimitives[ mesh->primitive ], 0, currentVertexCount );
		this->mesh->block = rspq_block_end();
	} 	rspq_block_run(this->mesh->block);

	finishRender();

}

void GL1Mesh::setupRender( Camera *camera ) {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( camera->poseMatrixInverse.e );
	if ( pose ) glMultMatrixf( pose->matrix.e );

	if ( material ) material->beginUse();
}

void GL1Mesh::finishRender() {

	if ( material ) material->endUse();

	glPopMatrix();

}
