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
#include <libdragon.h>

#include "GL1Pipeline.h"
#include "GL1Scene.h"
#include "GL1Text3D.h"

#include "console/console.h"

#include <GL/gl.h>
#include <GL/glu.h>

//#include <stb_image/stb_image.h>

using namespace ceres;

GL1Pipeline::GL1Pipeline() {
}

GL1Pipeline::~GL1Pipeline() {
}

bool GL1Pipeline::init( std::string &error ) {

	glFrontFace( GL_CCW );
	glEnable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_BLEND );
	glPointSize( 1.0 );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glPushMatrix();
	glScalef(0.1f, 0.1f, 0.1f);
	glPushMatrix();
	return true;
}

void GL1Pipeline::render( Scene *scene, Camera *camera ) {

	camera->updatePoseMatrixInverse();

	// Lighting setup

	GL1Scene *glScene = (GL1Scene *)scene;
	int32_t numLights = glScene->lights.size();

	for ( int32_t i = 0; i < numLights; i ++ ) glScene->lights[ i ]->updatePosition( camera );

	// Camera projection

	glMatrixMode( GL_PROJECTION );
	//glLoadMatrixf( camera->projectionMatrix.e );
	//gluPerspective(camera->fovYDegrees, camera->aspect, camera->minZ, camera->maxZ );
    glLoadIdentity();
	//float fov = deg2rad(camera->fovYDegrees);                      // transform fov from degrees to radians

    //float tangent = tanf(camera->fovYDegrees / 2.0f);               // tangent of half vertical fov
    //float height = camera->minZ * tangent;                 // half height of near plane
    //float width = height * camera->aspect;                  // half width of near plane
    //glFrustum(-width, width, -height, height, camera->minZ, camera->maxZ);
	gluPerspective(camera->fovYDegrees, camera->aspect, camera->minZ, camera->maxZ);
	// Render
	//glBindTexture( GL_TEXTURE_2D, 0 );

	scene->render( camera );

}

bool GL1Pipeline::finish( std::string &error ) {
	glPopMatrix();
	glPopMatrix();
	return true;
}

GL1Texture *GL1Pipeline::loadTexture( std::string filePath, std::string  &error ) {

	// Load PNG via stb_image
    int width, height, numChannels;

    //stbi_set_flip_vertically_on_load(true); // Flip img onload due to GL expecting top left (0,0)
    sprite_t *data = sprite_load( filePath.c_str() );
	if ( ! data ) {
		error = std::string( "Couldn't load " ) + filePath;
		return NULL;
	}

    GL1Texture *texture = new  GL1Texture();

	if ( ! texture->init( true, true, false, data, error ) ) {
		delete texture;
		error = std::string( "Couldn't load " ) + filePath;
		return NULL;
    }

    // Cleanup
    //stbi_image_free( data );
	//texture->data = NULL;

	return texture;

}
