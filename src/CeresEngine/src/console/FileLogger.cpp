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

#include "FileLogger.h"

using namespace ceres;

FileLogger::FileLogger() {
	path = "";
}

FileLogger::~FileLogger() {}

bool  FileLogger::init( std::string path ) {

	this->path = path;
	FILE *file = fopen( path.c_str(), "w" );
	if ( ! file ) return false;
	fclose( file );
	return true;

}

void FileLogger::log( std::string message ) {

	message += "\n";

	FILE *file = fopen( path.c_str(), "a" );
	if ( ! file ) return;
	fprintf( file,  "%s", message.c_str() );
	fclose( file );

}
