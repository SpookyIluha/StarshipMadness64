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

#include "FPFont.h"

using namespace ceres;

FPFont::FPFont() {
}

FPFont::~FPFont() {
	delete texture;
	if(RDPQfontLarge)
		rdpq_font_free(RDPQfontLarge);
	if(RDPQfontSmall)
		rdpq_font_free(RDPQfontSmall);
}

void FPFont::initWesternCharset() {

	// *****************************************************************************
	// Charset
	// *****************************************************************************

	charset.clear();

	charset.push_back( " " );
	charset.push_back( "A" );
	charset.push_back( "B" );
	charset.push_back( "C" );
	charset.push_back( "D" );
	charset.push_back( "E" );
	charset.push_back( "F" );
	charset.push_back( "G" );
	charset.push_back( "H" );
	charset.push_back( "I" );
	charset.push_back( "J" );
	charset.push_back( "K" );
	charset.push_back( "L" );
	charset.push_back( "M" );
	charset.push_back( "N" );
	charset.push_back( "O" );
	charset.push_back( "P" );
	charset.push_back( "Q" );
	charset.push_back( "R" );
	charset.push_back( "S" );
	charset.push_back( "T" );
	charset.push_back( "U" );
	charset.push_back( "V" );
	charset.push_back( "W" );
	charset.push_back( "X" );
	charset.push_back( "Y" );
	charset.push_back( "Z" );

	charset.push_back( "0" );
	charset.push_back( "1" );
	charset.push_back( "2" );
	charset.push_back( "3" );
	charset.push_back( "4" );
	charset.push_back( "5" );
	charset.push_back( "6" );
	charset.push_back( "7" );
	charset.push_back( "8" );
	charset.push_back( "9" );
	charset.push_back( "/" );
	charset.push_back( "(" );
	charset.push_back( ")" );
	charset.push_back( "." );
	charset.push_back( "-" );
	charset.push_back( "_" );
	charset.push_back( "+" );
	charset.push_back( "*" );
	charset.push_back( "=" );
	charset.push_back( ":" );
	charset.push_back( "!" );
	charset.push_back( "," );

	// *****************************************************************************
	// Character map to object indexes
	// *****************************************************************************

	characterIndexes.clear();

	int32_t index = 0;
	characterIndexes[ " " ] = index ++;
	characterIndexes[ "a" ] = index ++;
	characterIndexes[ "b" ] = index ++;
	characterIndexes[ "c" ] = index ++;
	characterIndexes[ "d" ] = index ++;
	characterIndexes[ "e" ] = index ++;
	characterIndexes[ "f" ] = index ++;
	characterIndexes[ "g" ] = index ++;
	characterIndexes[ "h" ] = index ++;
	characterIndexes[ "i" ] = index ++;
	characterIndexes[ "j" ] = index ++;
	characterIndexes[ "k" ] = index ++;
	characterIndexes[ "l" ] = index ++;
	characterIndexes[ "m" ] = index ++;
	characterIndexes[ "n" ] = index ++;
	characterIndexes[ "o" ] = index ++;
	characterIndexes[ "p" ] = index ++;
	characterIndexes[ "q" ] = index ++;
	characterIndexes[ "r" ] = index ++;
	characterIndexes[ "s" ] = index ++;
	characterIndexes[ "t" ] = index ++;
	characterIndexes[ "u" ] = index ++;
	characterIndexes[ "v" ] = index ++;
	characterIndexes[ "w" ] = index ++;
	characterIndexes[ "x" ] = index ++;
	characterIndexes[ "y" ] = index ++;
	characterIndexes[ "z" ] = index ++;

	// Upper-case maps to same indexes as lower-case
	index = 0;
	characterIndexes[ " " ] = index ++;
	characterIndexes[ "A" ] = index ++;
	characterIndexes[ "B" ] = index ++;
	characterIndexes[ "C" ] = index ++;
	characterIndexes[ "D" ] = index ++;
	characterIndexes[ "E" ] = index ++;
	characterIndexes[ "F" ] = index ++;
	characterIndexes[ "G" ] = index ++;
	characterIndexes[ "H" ] = index ++;
	characterIndexes[ "I" ] = index ++;
	characterIndexes[ "J" ] = index ++;
	characterIndexes[ "K" ] = index ++;
	characterIndexes[ "L" ] = index ++;
	characterIndexes[ "M" ] = index ++;
	characterIndexes[ "N" ] = index ++;
	characterIndexes[ "O" ] = index ++;
	characterIndexes[ "P" ] = index ++;
	characterIndexes[ "Q" ] = index ++;
	characterIndexes[ "R" ] = index ++;
	characterIndexes[ "S" ] = index ++;
	characterIndexes[ "T" ] = index ++;
	characterIndexes[ "U" ] = index ++;
	characterIndexes[ "V" ] = index ++;
	characterIndexes[ "W" ] = index ++;
	characterIndexes[ "X" ] = index ++;
	characterIndexes[ "Y" ] = index ++;
	characterIndexes[ "Z" ] = index ++;

	characterIndexes[ "0" ] = index ++;
	characterIndexes[ "1" ] = index ++;
	characterIndexes[ "2" ] = index ++;
	characterIndexes[ "3" ] = index ++;
	characterIndexes[ "4" ] = index ++;
	characterIndexes[ "5" ] = index ++;
	characterIndexes[ "6" ] = index ++;
	characterIndexes[ "7" ] = index ++;
	characterIndexes[ "8" ] = index ++;
	characterIndexes[ "9" ] = index ++;
	characterIndexes[ "/" ] = index ++;
	characterIndexes[ "(" ] = index ++;
	characterIndexes[ ")" ] = index ++;
	characterIndexes[ "." ] = index ++;
	characterIndexes[ "-" ] = index ++;
	characterIndexes[ "_" ] = index ++;
	characterIndexes[ "+" ] = index ++;
	characterIndexes[ "*" ] = index ++;
	characterIndexes[ "=" ] = index ++;
	characterIndexes[ ":" ] = index ++;
	characterIndexes[ "!" ] = index ++;
	characterIndexes[ "," ] = index ++;

	newlineIndex = index;

}

void FPFont::generateUVs( std::vector<Vector2> characterSizes, FPTexture *texture ) {

	uint32_t numChars = charset.size();

	this->characterSizes = characterSizes;

	float uPixel = 1.0 / ((float)texture->resolutionX);
	float vPixel = 1.0 / ((float)texture->resolutionY);

	characterUV0.clear();
	characterUV1.clear();
	Vector2 uv0, uv1;
	float y = 0;
	for ( uint32_t i = 0; i < numChars; i ++ ) {
		float dU = characterSizes[ i ].x * uPixel;
		float dV = characterSizes[ i ].y * vPixel;

		uv1.x += dU;
		uv1.y = y + dV;

		if ( uv1.x >= 1 ) {
			uv0.x = 0;
			uv1.x = dU;
			uv0.y = y + dV;
			uv1.y = uv0.y + dV;
			y += dV;
		}

		uv0.x += uPixel;
		uv1.x += uPixel;
		characterUV0.push_back( uv0 );
		characterUV1.push_back( uv1 );
		uv1.x -= uPixel;
		uv0.x = uv1.x;

	}

	this->texture = texture;

}
