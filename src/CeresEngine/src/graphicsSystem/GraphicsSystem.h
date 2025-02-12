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

#include <string>
#include <vector>

#include "image/Image.h"

namespace ceres {

// A graphics mode
struct GraphicsMode {

	// Resolution
	int32_t resolutionX;
	int32_t resolutionY;

	// Screen dimensions
	float dimensionXmm;
	float dimensionYmm;

	float framesPerSecond;

	GraphicsMode();

	bool validate();

};

// Graphics system (acess to graphics hardware)
class GraphicsSystem {
public:

	GraphicsSystem() {}
	virtual ~GraphicsSystem() {}

	// Init graphics system. Should init screen and show initially a black image, or remain in blanking state.
	virtual bool init( std::string &error ) = 0;

	// Close system. Should turn off screen (blanking mode).
	virtual bool close() = 0;

	// Graphics modes functions
	virtual std::vector<GraphicsMode> getGraphicsModesList() = 0;
	virtual bool setCurrentGraphicsMode( GraphicsMode *graphicsMode, std::string &error ) = 0;
	virtual GraphicsMode *getCurrentGraphicsMode() = 0;

	// Framebuffer access. Must be re-called each frame
	virtual Image *getFramebuffer() = 0;

	// Send current framebuffer to the screen
	virtual void presentFrameBuffer() = 0;

	// Blanking (disable screen)
	virtual bool setBlanking( bool blankingOn ) = 0;
	virtual bool getBlanking() = 0;

};

}
