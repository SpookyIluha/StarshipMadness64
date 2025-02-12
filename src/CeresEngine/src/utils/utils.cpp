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

#include "utils.h"

#include <math.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "console/console.h"

#include <dirent.h>

bool ceres::fileExists( const char *path ) {

	struct stat stat_buf;
	int rc = stat( path, &stat_buf );
	if ( rc ) return false;
	return true;

}

bool ceres::isBeneath( std::string path, std::string base ) {

	return stringStartsWith( path, base );

}

std::string ceres::getParentDirectory( std::string path ) {

	if ( stringEndsWith( path, "/" ) ) path = path.substr( 0, path.length() - 1 );

	unsigned int pathLastIndexOfSlash = path.rfind( "/", path.length() );

	if ( pathLastIndexOfSlash < path.length() ) {

		return path.substr( 0, pathLastIndexOfSlash );

	}
	else return path;

}

std::string ceres::concatPaths( std::string path1, std::string path2 ) {

	bool addSlash = ! stringEndsWith( path1, "/" );

	std::string s = path1;
	if ( addSlash ) s += "/";
	s += path2;
	return s;

}

std::string ceres::getFilenameExtension( std::string path ) {

	unsigned int pathLastIndexOfDot = path.rfind( ".", path.length() );

	if ( pathLastIndexOfDot > 0 && ! ( pathLastIndexOfDot == 1 && stringStartsWith( path, ".." ) ) && path.length() > pathLastIndexOfDot + 1 ) {

		return path.substr( pathLastIndexOfDot + 1 );

	}
	else return "";

}

std::string ceres::removeFilenameExtension( std::string path ) {


	unsigned int pathLastIndexOfDot = path.rfind( ".", path.length() );

	if ( pathLastIndexOfDot > 0 && ! ( pathLastIndexOfDot == 1 && stringStartsWith( path, ".." ) ) && path.length() > pathLastIndexOfDot + 1 ) {

		return path.substr( 0, pathLastIndexOfDot );

	}
	else return "";

}

std::string ceres::removePathFromFilename( std::string path ) {

	unsigned int pathLastIndexOfSlash = path.rfind( "/", path.length() );

	if ( pathLastIndexOfSlash > 0 && path.length() > pathLastIndexOfSlash + 1 ) {

		return path.substr( pathLastIndexOfSlash + 1 );

	}
	else return path;

}

std::string ceres::removeFilenameFromPath( std::string path ) {

	unsigned int pathLastIndexOfSlash = path.rfind( "/", path.length() );

	if ( pathLastIndexOfSlash > 0 && path.length() > pathLastIndexOfSlash + 1 ) {

		return path.substr( 0, pathLastIndexOfSlash );

	}
	else return path;

}

std::string ceres::stringLimitLength( std::string s, int32_t length ) {

	std::string output;
	int32_t numChars = 0;
	for ( uint32_t i = 0; i < s.length(); i ++ ) {
		if ( s[ i ] != '\n' ) numChars ++;
		else {
			numChars = 0;
			output += s[ i ];
			continue;
		}

		if ( numChars == length ) {
			output += std::string( "\n" );
			numChars = 0;
		}
		output += s[ i ];
	}

	return output;
}

float *ceres::convertRGB888ToRGBAFloat( uint8_t *rgb, int32_t resX, int32_t resY ) {

	int32_t numFloats = 4 * resX * resY;
	float *result = new float[ numFloats ];

	int32_t p = 0;
	for ( int32_t i = 0; i < numFloats; i += 4 ) {

		result[ i ] = rgb[ p++ ] / 255.0f;
		result[ i + 1 ] = rgb[ p++ ] / 255.0f;
		result[ i + 2 ] = rgb[ p++ ] / 255.0f;
		result[ i + 3 ] = 1.0f;

	}

	return result;

}

// Obsolete, don't use:
uint8_t *ceres::loadPPMImage( const char *path, int32_t &resX, int32_t &resY ) {

	std::ifstream ifs( path );

	std::string str;
	if ( ! std::getline( ifs, str ) || str != "P6" ) return NULL;
	if ( ! std::getline( ifs, str ) ) return NULL;
	if ( str[ 0 ] == '#' ) {
		if ( ! std::getline( ifs, str ) ) return NULL;
	}

	std::istringstream iss( str );

	std::string token;
	int32_t i = 0;
	resX = 0;
	resY = 0;
    while ( std::getline( iss, token, ' ' ) ) {

		if ( i == 0 ) resX = std::stoi( token );
		else if ( i == 1 ) resY = std::stoi( token );

		i++;

    }

	if ( resX <= 0 || resY <= 0 ) return NULL;

	if ( ! std::getline( ifs, str ) || str != "255" ) return NULL;

	std::streamsize bufLength = resX * resY * 3;
	uint8_t *buf = new uint8_t[ bufLength ];

	std::streamsize read = ifs.read( (char *)buf, bufLength ).gcount();
	if ( read != bufLength ) {

		delete [] buf;
		return NULL;

	}

	return buf;

}

bool ceres::stringStartsWith( std::string const &s1, std::string const &s2 ) {

	return s1.rfind( s2, 0) == 0;

}

bool ceres::stringEndsWith( std::string const &s1, std::string const &s2 ) {

	if ( s1.length() >= s2.length() ) return ( 0 == s1.compare( s1.length() - s2.length(), s2.length(), s2 ) );
	return false;

}

std::string ceres::numberToHumanReadable( int32_t number ) {

	int32_t numDecimals = 2;

	double limit = 1000000.0;

	if ( number < limit ) return std::to_string( number );

	double decimal = number / limit;
	std::string unit = "M";
	int32_t unitNumber = 0;

	while ( ( decimal >= 1000.0 ) && ( unitNumber < 3 ) ) {

		limit *= 1000;
		unitNumber ++;
		switch ( unitNumber ) {
			case 1:
				unit = "G";
				break;
			case 2:
				unit = "T";
				break;
			case 3:
				unit = "E";
				break;
			default:
				//Nothing to do
				break;
		}

		decimal = number / limit;

	}

	std::stringstream stream;
	stream << std::fixed << std::setprecision( numDecimals ) << decimal;

	return std::string( stream.str().c_str() ) + unit;

}

std::string ceres::numberOfBytesToHumanReadable( int32_t numberOfBytes ) {

	int32_t numDecimals = 2;

	double limit = 1024 * 1024;

	if ( numberOfBytes < limit ) return std::to_string( numberOfBytes );

	double decimal = numberOfBytes / limit;
	std::string unit = " megabytes";
	int32_t unitNumber = 0;

	while ( ( decimal >= 1024.0 ) && ( unitNumber < 3 ) ) {

		limit *= 1024;
		unitNumber ++;
		switch ( unitNumber ) {
			case 1:
				unit = " gigabytes";
				break;
			case 2:
				unit = " terabytes";
				break;
			case 3:
				unit = " exabytes";
				break;
			default:
				//Nothing to do
				break;
		}

		decimal = numberOfBytes / limit;

	}

	std::stringstream stream;
	stream << std::fixed << std::setprecision( numDecimals ) << decimal;

	return std::string( stream.str().c_str() ) + unit;

}

float ceres::reducePrecision( float input, int32_t precision ) {

	return round( input * pow( 10, precision ) ) * pow( 0.1, precision );

}

std::string ceres::formatDouble( double value, int32_t precision ) {

	std::stringstream stream;
	stream << std::fixed << std::setprecision( precision ) << value;

	return std::string( stream.str().c_str() );

}

std::string ceres::timeInSecondsToHumanReadable( float time ) {

	const int precision = 2;

	std::string s = "";

	if ( time < 0.0 ) {

		time = - time;
		s += "-";

	}

	if ( time < 1.0 ) {

		time *= 1000.0;
		return s + std::to_string( reducePrecision( time, precision ) ) + " ms";

	}

	if ( time < 60.0 ) {

		return s + std::to_string( reducePrecision( time, precision ) ) + " s";

	}

	time /= 60.0;

	if ( time < 60.0 ) {

		return s + std::to_string( reducePrecision( time, precision ) ) + " minutes";

	}

	time /= 60.0;

	if ( time < 24.0 ) {

		return s + std::to_string( reducePrecision( time, precision ) ) + " hours";

	}

	time /= 24.0;

	if ( time < 365.0 ) {

		return s + std::to_string( reducePrecision( time, precision ) ) + " days";

	}

	return s + std::to_string( reducePrecision( time, precision ) ) + " years";

}

bool ceres::getEndianness( bool &bigEndian ) {

	uint16_t word = 0x1234;
	uint8_t *byte = (uint8_t*) &word;
	if ( *byte == 0x12 ) {

		bigEndian = true;
		return true;

	}
	else if ( *byte == 0x34 ) {

		bigEndian = false;
		return true;

	}
	else return false;

}

std::string ceres::getLicense() {

	return

		std::string( "Copyright © 2024 yomboprime https://codeberg.org/yombo\n" ) +
		std::string( "    This source code is free software; you can redistribute it\n" ) +
		std::string( "    and/or modify it in source code form under the terms of the GNU\n" ) +
		std::string( "    General Public License as published by the Free Software\n" ) +
		std::string( "    Foundation; either version 2 of the License, or (at your option)\n" ) +
		std::string( "    any later version.\n" ) +
 		std::string( "    \n" ) +
		std::string( "    This program is distributed in the hope that it will be useful,\n" ) +
		std::string( "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n" ) +
		std::string( "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" ) +
		std::string( "    GNU General Public License for more details.\n" ) +
		std::string( "    You should have received a copy of the GNU General Public License\n" ) +
		std::string( "    along with this program; if not, write to the Free Software\n" ) +
		std::string( "    Foundation, Inc.,\n" ) +
		std::string( "    59 Temple Place - Suite 330, Boston, MA 02111-1307, USA\n" ) +
		std::string( "    " );

}
