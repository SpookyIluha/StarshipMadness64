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

//#include <kos.h>
//#include <kos/img.h>

#include <libdragon.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
//#include <GL/glext.h>
//#include <GL/glkos.h>

//#include <dc/video.h>
//#include <dc/sound/sound.h>
//#include <dc/sound/sfxmgr.h>
//#include <wav/sndwav.h>

//#include <dc/biosfont.h>

#include "math3d/MathUtils.h"
#include "console/console.h"
#include "time/time.h"

#include "SpaceGameSimulation.h"

using namespace ceres;

int main( int argc, char **argv ) {

	debug_init_isviewer();
	debug_init_usblog();

	debugf( "Starting...\n\n" );
	
	asset_init_compression(2);

    dfs_init(DFS_DEFAULT_LOCATION);

    display_init(((resolution_t){.width = 640, .height = 480, .interlaced = INTERLACE_HALF}), DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_DEDITHER);

    timer_init();
    rdpq_init();
    joypad_init();
    audio_init(32000, 6);
    mixer_init(12);
    srand(getentropy32());
    register_VI_handler((void(*)())rand);
	gl_init();

    ceres::font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(1, ceres::font);

	Simulation *simulation = new SpaceGameSimulation();

	debugf( "\n" );
	debugf( simulation->getTitle().c_str() );


	std::string error;
	if ( ! simulation->init( error ) ) {
		debugf( "Error initing: %s" , error.c_str() );
		return -1;
	}


	float t0 = getTime();
    while ( 1 ) {

		float t = getTime();
		float dt = t - t0;
		t0 = t;

        updateRumble();

        Simulation::SimulationStepResult result = simulation->timestep( dt, t );

		if ( result == Simulation::SIM_TERMINATE ) break;


    }

    if ( ! simulation->finish( error ) ) {
		debugf( "Error finishing: %s" , error.c_str() );
		return -1;
	}

    timer_close();
    rdpq_close();
    joypad_close();
    audio_close();
    mixer_close();

    joypad_set_rumble_active(JOYPAD_PORT_1, false);
    unregister_VI_handler((void(*)())rand);
    display_close();
    return 0;
}
