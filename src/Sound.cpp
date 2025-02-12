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

#include <wav/sndwav.h>
#include "SpaceshipMadnessConfig.h"

#include "console/console.h"

using namespace ceres;

Sound::Sound() :
	playingMusic( false ),
	musicTrack( -1 ),
	audioEnabled( true ),
	musicEnabled( true ) {
}

Sound::~Sound() {
}

bool Sound::loadAudio( std::string name, std::string &error ) {

	std::string path = SPACESHIP_MADNESS_DIR + std::string( "audios/" ) + name + std::string( ".wav" );

	if ( audioEnabled ) {
		sfxhnd_t audio = snd_sfx_load( path.c_str() );
		if ( audio == SFXHND_INVALID ) {
			error = std::string( "Could not load '" ) + name + std::string( ".wav'" );
			return false;
		}
		audios[ name ] = audio;
	}

	return true;

}

void Sound::unloadAudios() {

	snd_sfx_unload_all();
	audios.clear();

}

bool Sound::playAudio( std::string name, float volume, float panning ) {

	// volume: 0..1
	// panning: -1..1 (0 center)

	if ( audioEnabled ) {
		sfxhnd_t audio = audios[ name ];
		if ( ! audio ) return false;
		snd_sfx_play( audio, volume * 255.0, ( panning + 1.0 ) * 128.0 );
	}

	return true;

}

bool Sound::playMusic( std::string path ) {

	if ( ! musicEnabled ) return true;

	if ( playingMusic ) stopMusic();

	musicTrack = wav_create( path.c_str(), true );

	if ( musicTrack < 0 ) return false;

	wav_volume( musicTrack, 60 );
	wav_play( musicTrack );

	playingMusic = true;

	return true;

}

bool Sound::stopMusic() {

	if ( ! musicEnabled || ! playingMusic ) return true;

	if ( musicTrack < 0 ) return false;

	wav_stop( musicTrack );
	wav_destroy( musicTrack );

	playingMusic = false;

	musicTrack = 0;

	return true;

}
