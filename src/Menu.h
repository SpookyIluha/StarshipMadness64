/*
 * Copyright © 2020 - 2024 yomboprime https://codeberg.org/yombo
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

#include "math3d/Scene.h"
#include "fixedPipeline/gl1/GL1Text3D.h"
#include "game/Controller.h"
#include "Sound.h"

namespace ceres {

class MenuEntry {
public:
	MenuEntry() {}
	MenuEntry( int32_t value, std::string text );
	virtual ~MenuEntry() {}

	int32_t value;
	std::string text;

	GL1Text3D *object;

};


class MenuList {
public:
	MenuList() {}
	virtual ~MenuList();

	std::vector<MenuEntry *> entries;
};

class Menu : public Scene {
public:

	Menu();
	virtual ~Menu();

	void timestep( float dt, float time );

	bool createMenus( FPFont *font, Sound *sound );
	void updateMenusVisibility();

	void show( bool gamePlaying );
	void hide();
	void up();
	void down();
	void select();

	bool isShown;
	int32_t currentMenu;
	int32_t currentSelection;

	int32_t result;

	std::vector<MenuList *> menus;

	Camera *camera;
	Controller *controller;
	bool previousFramePressed;
	float nextTimePressed;

	Sound *sound;

};

}
