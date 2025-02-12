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

#include <stdint.h>
#include <string>
#include <vector>

#include <libdragon.h>

namespace ceres {

// File functions

bool fileExists( const char *path );
bool isBeneath( std::string path, std::string base );
std::string getParentDirectory( std::string path );
std::string concatPaths( std::string path1, std::string path2 );
std::string getFilenameExtension( std::string path );
std::string removeFilenameExtension( std::string path );
std::string removePathFromFilename( std::string path );
std::string removeFilenameFromPath( std::string path );

std::string stringLimitLength( std::string s, int32_t length );

float *convertRGB888ToRGBAFloat( uint8_t *rgb, int32_t resX, int32_t resY );
// Obsolete, don't use:
uint8_t *loadPPMImage( const char *path, int32_t &resX, int32_t &resY );


// String functions

bool stringStartsWith( std::string const &s1, std::string const &s2 );
bool stringEndsWith( std::string const &s1, std::string const &s2 );

std::string numberToHumanReadable( int32_t number );
std::string numberOfBytesToHumanReadable( int32_t numberOfBytes );
std::string timeInSecondsToHumanReadable( float time );


// Misc functions

bool getEndianness( bool &bigEndian );

float reducePrecision( float input, int32_t precision );
std::string formatDouble( double value, int32_t precision );

std::string getLicense();


// Byte swap unsigned short
uint16_t byteswap_uint16( uint16_t val );

// Byte swap short
int16_t byteswap_int16( int16_t val );

// Byte swap unsigned int
uint32_t byteswap_uint32( uint32_t val );

// Byte swap int
int32_t byteswap_int32( int32_t val );

// Byte swap unsigned long long
int64_t byteswap_int64( int64_t val );

// Byte swap unsigned long long
uint64_t byteswap_uint64( uint64_t val );

float byteswap_float( const float inFloat );

extern  rdpq_font_t* font;

}
