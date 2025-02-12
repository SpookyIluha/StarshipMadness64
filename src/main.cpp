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

#include <kos.h>
#include <kos/img.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glkos.h>

#include <dc/video.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <wav/sndwav.h>

#include <dc/biosfont.h>

#include "math3d/MathUtils.h"
#include "console/console.h"
#include "time/time.h"

#include "SpaceGameSimulation.h"

using namespace ceres;

int main( int argc, char **argv ) {

	dbglog_set_level( DBG_WARNING );

	println( "Starting...\n\n" );

    snd_init();
	snd_stream_init();
    wav_init();
	glKosInit();

	Simulation *simulation = new SpaceGameSimulation();

	println( "\n" );
	println( simulation->getTitle() );

	vid_border_color( 0, 0, 0 );

	std::string error;
	if ( ! simulation->init( error ) ) {
		println( std::string( "Error initing: " ) + error );
		return -1;
	}

	vid_border_color( 0, 0, 0 );

	float t0 = getTime();
    while ( 1 ) {

		float t = getTime();
		float dt = t - t0;
		t0 = t;

        Simulation::SimulationStepResult result = simulation->timestep( dt, t );

		if ( result == Simulation::SIM_TERMINATE ) break;

        glKosSwapBuffers();

    }

    if ( ! simulation->finish( error ) ) {
		println( std::string( "Error finishing: " ) + error );
		return -1;
	}

	wav_shutdown();
    snd_stream_shutdown();

	snd_sfx_unload_all();
    snd_shutdown();

    return 0;
}
