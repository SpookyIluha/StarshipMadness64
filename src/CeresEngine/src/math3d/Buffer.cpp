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

#include "Buffer.h"

using namespace ceres;

Buffer::Buffer() {

	size = 0;
	start = 0;
	length = 0;
	buffer = NULL;

}


Buffer::~Buffer() {
	delete [] buffer;
}

bool Buffer::init( int32_t size, std::string &error ) {

	if ( size <= 0 ) {
		error = "Error: tried to create a circular buffer with size <= 0.";
		return false;
	}

	this->size = size;

	buffer = new float[ size ];
	if ( ! buffer ) {
		error = "Not enough memory to create circular buffer.";
		this->size = 0;
		return false;
	}

	return true;

}
