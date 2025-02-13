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

#include "Menu.h"

#include "fixedPipeline/gl1/GL1Pipeline.h"
#include "fixedPipeline/gl1/GL1ObjectUtils.h"
#include "fixedPipeline/gl1/GL1Mesh.h"
#include "fixedPipeline/gl1/GL1Material.h"

#include "console/console.h"
#include <fixedPipeline/gl1/GL1Light.h>

using namespace ceres;

MenuEntry::MenuEntry( int32_t value, std::string text ) :
	value( value ),
	text( text ) {
}

MenuList::~MenuList() {
	for ( uint32_t i = 0; i < entries.size(); i ++ ) delete entries[ i ];
}

Menu::Menu() :
	isShown( false ),
	currentSelection( 0 ) {
}

Menu::~Menu() {
	for ( uint32_t i = 0; i < menus.size(); i ++ ) delete menus[ i ];
}

void Menu::show( bool gamePlaying ) {

	if ( isShown ) return;

	currentMenu = gamePlaying ? 1 : 0;
	currentSelection = 0;
	isShown = true;
	previousFramePressed = false;
	result = -1;

	updateMenusVisibility();

	sound->playAudio( std::string( "menu1" ) );

}

void Menu::updateMenusVisibility() {
	for ( int32_t i = 0, n = menus.size(); i < n; i ++  ) {
		MenuList *list = menus[ i ];
		for ( int32_t j = 0, n2 = list->entries.size(); j < n2; j ++  ) {
			list->entries[ j ]->object->visible = currentMenu == i;
		}
	}
}

void Menu::hide() {
	isShown = false;
	sound->playAudio( std::string( "menu2" ) );
}

void Menu::up() {

	if ( ! isShown ) return;

	sound->playAudio( std::string( "menu1" ) );

	if ( currentSelection > 0 ) currentSelection --;

}

void Menu::down() {

	if ( ! isShown ) return;

	sound->playAudio( std::string( "menu1" ) );

	if ( (uint32_t)currentSelection < menus[ currentMenu ]->entries.size() - 1 ) currentSelection ++;

}

void Menu::timestep( float dt, float time ) {

	if ( ! isShown ) return;

	if ( previousFramePressed ) {
		if ( time > nextTimePressed ) previousFramePressed = false;
	} else {
		bool pressed = true;
		int32_t currentMenu1 = currentMenu;
		int32_t currentSelection1 = currentSelection;
		previousFramePressed = true;
		if ( controller->up ) up();
		else if ( controller->down ) down();
		else if ( controller->a ) select();
		else if ( controller->b ) hide();
		else pressed = false;
		if ( pressed ) {
			menus[ currentMenu1 ]->entries[ currentSelection1 ]->object->material->diffuse.set( 1, 1, 1 );
			nextTimePressed = time + 0.3;
		}
		previousFramePressed = pressed;
	}

	MenuList *currentMenuList = menus[ currentMenu ];
	for ( int32_t i = 0, n = currentMenuList->entries.size(); i < n; i ++ ) {
		MenuEntry *entry = currentMenuList->entries[ i ];
		if ( i == currentSelection ) entry->object->material->diffuse.set( 0.1, 0.3, 1 );
		else entry->object->material->diffuse.set( 1, 1, 1 );
	}

}

bool Menu::createMenus( FPFont *font, Sound *sound ) {

	camera = new Camera();
	this->sound = sound;

	MenuList *menu = new MenuList();
	menu->entries.push_back( new MenuEntry( 0, "Start new game" ) );
	menu->entries.push_back( new MenuEntry( 1, "Instructions" ) );
	menu->entries.push_back( new MenuEntry( 2, "About" ) );
	menu->entries.push_back( new MenuEntry( 5, "Toggle music" ) );
	menu->entries.push_back( new MenuEntry( -1, "Exit game" ) );
	menus.push_back( menu );

	menu = new MenuList();
	menu->entries.push_back( new MenuEntry( -1, "Resume game" ) );
	menu->entries.push_back( new MenuEntry( -1, "Abort game" ) );
	menus.push_back( menu );

	menu = new MenuList();
	menu->entries.push_back( new MenuEntry( 3, "Yes, abort game" ) );
	menu->entries.push_back( new MenuEntry( -1, "No" ) );
	menus.push_back( menu );

	menu = new MenuList();
	menu->entries.push_back( new MenuEntry( 4, "Yes, exit game" ) );
	menu->entries.push_back( new MenuEntry( -1, "No" ) );
	menus.push_back( menu );

	std::string error;

	for ( int32_t i = 0, n = menus.size(); i < n; i ++ ) {

		MenuList *list = menus[ i ];

		for ( int32_t j = 0, n2 = list->entries.size(); j < n2; j ++ ) {

			MenuEntry *entry = list->entries[ j ];

			GL1Material *textMaterial = new GL1Material();
			textMaterial->illuminated = false;
			textMaterial->diffuse.set( 1, 1, 1 );
			textMaterial->texture = font->texture;
			textMaterial->transparent = true;
			textMaterial->opacity = 1.0;
			textMaterial->depthTest = false;

			GL1Text3D *text = new GL1Text3D();
			text->pose = new Pose();
			text->pose->position.set( 60, 120 + j * 70, - 10 );
			text->pose->updateMatrix();
			text->pose->scale = 2;
			text->init( font, textMaterial, Vector3( 1, 1, 1 ) );
			text->setString( entry->text );
			objects.push_back( text );

			entry->object = text;

		}
	}

	return true;

}

void Menu::select() {

	bool doCurrentSelection = false;

	bool playSelect = false;
	bool playAbort = false;

	switch ( currentMenu ) {

		case 0:

			switch ( currentSelection ) {
				case 0:
				case 1:
				case 2:
				case 3:
					doCurrentSelection = true;
					playSelect = true;
					break;
				case 4:
					currentMenu = 3;
					currentSelection = 0;
					playSelect = true;
					updateMenusVisibility();
					break;
				default:
					break;
			}

			break;

		case 1:

			switch ( currentSelection ) {
				case 0:
					playAbort = true;
					isShown = false;
					break;
				case 1:
					currentMenu = 2;
					currentSelection = 0;
					playSelect = true;
					updateMenusVisibility();
					break;
				default:
					break;
			}

			break;

		case 2:

			switch ( currentSelection ) {
				case 0:
					doCurrentSelection = true;
					playSelect = true;
					break;
				case 1:
					isShown = false;
					playAbort = true;
					break;
				default:
					break;
			}

			break;

		case 3:

			switch ( currentSelection ) {
				case 0:
					doCurrentSelection = true;
					playSelect = true;
					break;
				case 1:
					isShown = false;
					playAbort = true;
					break;
				default:
					break;
			}

			break;

		default:
			break;

	}

	if ( playSelect ) sound->playAudio( std::string( "menu1" ) );
	else if ( playAbort ) sound->playAudio( std::string( "menu2" ) );

	if ( doCurrentSelection ) result = menus[ currentMenu ]->entries[ currentSelection ]->value;
	else result = -1;

}
