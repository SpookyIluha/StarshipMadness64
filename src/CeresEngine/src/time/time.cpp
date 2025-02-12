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
#include <libdragon.h>

namespace ceres {


    double getTime() {

        //clock_gettime( CLOCK_MONOTONIC, &t );
        double ticks = timer_ticks();
    
        //return ( (double) t.tv_sec ) + ( ( double ) t.tv_nsec ) * 0.000000001;
        return TICKS_TO_MS(ticks) / 1000.0f;
    }
    
    void sleepNanos( int32_t nanos ) {
    
        // max nanos: 1000000000 - 1
        int32_t t = TICKS_FROM_US(nanos / 1000);
        kthread_sleep(t);
    
    }
    

}
