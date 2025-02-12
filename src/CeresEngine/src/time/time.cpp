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

#include <time.h>
#include <inttypes.h>

#include "time/time.h"

namespace ceres {

double getTime() {

	struct timespec t;

	clock_gettime( CLOCK_MONOTONIC, &t );

	return ( (double) t.tv_sec ) + ( ( double ) t.tv_nsec ) * 0.000000001;

}

void sleepNanos( int32_t nanos ) {

	// max nanos: 1000000000 - 1

    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = nanos;
    nanosleep( &t, NULL );

}

}
