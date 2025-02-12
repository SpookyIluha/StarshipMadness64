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

#include "Sound.h"

#include <libdragon.h>
#include "SpaceshipMadnessConfig.h"

#include "console/console.h"

using namespace ceres;

Sound::Sound() :
	playingMusic( false ),
	audioEnabled( true ),
	musicEnabled( true ) {
}

Sound::~Sound() {
}

bool Sound::loadAudio( std::string name, std::string &error ) {

	std::string path = SPACESHIP_MADNESS_DIR + std::string( "audios/" ) + name + std::string( ".wav64" );

	if ( audioEnabled ) {
		wav64_t* audio = (wav64_t*)malloc(sizeof(wav64_t)); wav64_open(audio, path.c_str() );
		audios[ name ] = audio;
	}

	return true;

}

void Sound::unloadAudios() {

	//snd_sfx_unload_all();
	audios.clear();

}

bool Sound::playAudio( std::string name, float volume, float panning ) {

	// volume: 0..1
	// panning: -1..1 (0 center)

	panning = (panning + 1.0) * 0.5f; // to 0..1 range

	if ( audioEnabled ) {
		wav64_t* audio = audios[ name ];
		if ( ! audio ) return false;
		wav64_play(audio, 2 + ((name[0] % 4) * 2)); // volume * 255.0, ( panning + 1.0 ) * 128.0 );
		float volume_l = 1 - panning;
		float volume_r = panning;
		mixer_ch_set_vol(2, volume_l, volume_r);
		//channel++;
		//if(channel > 4) channel = 0;
	}

	return true;

}

bool Sound::playMusic( std::string path ) {

	if ( ! musicEnabled ) return true;

	if ( playingMusic ) stopMusic();

	 wav64_open(&musicTrack, path.c_str());

	//if ( musicTrack < 0 ) return false;

	//wav_volume( musicTrack, 60 );
	wav64_set_loop(&musicTrack, true);
	wav64_play( &musicTrack, 0 );
	
	playingMusic = true;

	return true;

}

bool Sound::stopMusic() {

	if ( ! musicEnabled || ! playingMusic ) return true;

	//if ( musicTrack < 0 ) return false;

	mixer_ch_stop( 0 );
	wav64_close( &musicTrack );

	playingMusic = false;

	memset(&musicTrack, 0, sizeof(wav64_t));

	return true;

}
