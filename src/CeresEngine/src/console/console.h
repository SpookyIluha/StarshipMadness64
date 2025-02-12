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

#include <stdio.h>
#include <string>

namespace ceres {

void print( const char *str, FILE *file = NULL );
void print( std::string str, FILE *file = NULL );
void println( const char *str, FILE *file = NULL );
void println( std::string str, FILE *file = NULL );
void println( FILE *file = NULL );

void bold( bool on );
void underlined( bool on );

void textProgressBar( int progress, int maxProgress, std::string messageLine1, std::string messageLine2, int barSize );
void textProgressBarFinish( std::string messageLine1, std::string messageLine2, int barSize );

}
