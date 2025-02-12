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

#include "console.h"

void ceres::print( const char *str, FILE *file ) {

	if ( file ) fprintf( file,  "%s", str );
	else printf( "%s", str );

}

void ceres::print( std::string str, FILE *file ) {

	if ( file ) fprintf( file, "%s", str.c_str() );
	else printf( "%s", str.c_str() );
}

void ceres::println( const char *str, FILE *file ) {

	if ( file ) fprintf( file, "%s\n", str );
	else printf( "%s\n", str );

}

void ceres::println( std::string str, FILE *file ) {

	if ( file ) fprintf( file, "%s\n", str.c_str() );
	else printf( "%s\n", str.c_str() );

}

void ceres::println( FILE *file ) {

	if ( file ) fprintf( file, "\n" );
	else printf( "\n" );

}

void ceres::bold( bool on ) {

	// TODO

}

void ceres::underlined( bool on ) {

	// TODO

}

void ceres::textProgressBar( int progress, int maxProgress, std::string messageLine1, std::string messageLine2, int barSize ) {

	std::string s = messageLine1 + "[";

	double fraction = 0.0;
	if ( maxProgress > 0.0 ) {

		fraction = ((double)progress) / ((double)maxProgress);

	}

	int numProgressBars = (int)( fraction * barSize );
	if ( numProgressBars < 0 ) numProgressBars = 0;
	if ( numProgressBars > barSize ) numProgressBars = barSize;

	for ( int i = 0; i < numProgressBars; i++ ) {
		s = s + "=";
	}
	for ( int i = numProgressBars; i < barSize; i++ ) {
		s = s + " ";
	}

	int percent = (int)( fraction * 100 );

	s = s + "] ";
	s = s + std::to_string( percent );
	s = s + "%\n";

	s = s + messageLine2;

	s = s + "\r\x1b[A";

	println( s );
	fflush( stdout );

}

void ceres::textProgressBarFinish( std::string messageLine1, std::string messageLine2, int barSize ) {

	std::string s = "\r\x1b[A";
	int n = messageLine1.length() + barSize + 7 + messageLine2.length();
	for ( int i = 0; i < n; i++ ) {

		s = s + " ";

	}

	println( s + "\n\n\n" );

}
