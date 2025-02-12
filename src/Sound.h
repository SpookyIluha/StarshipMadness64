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

#pragma once

#include <string>
#include <vector>
#include <map>

#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/stream.h>

namespace ceres {

class Sound {
public:

	Sound();
	virtual ~Sound();


	virtual bool loadAudio( std::string name, std::string &error );
	virtual void unloadAudios();
	virtual bool playAudio( std::string name, float volume = 0.9, float panning = 0.0 );

	virtual bool playMusic( std::string path );
	virtual bool stopMusic();

	std::map<std::string, sfxhnd_t> audios;

	bool playingMusic;
	snd_stream_hnd_t musicTrack;

	bool audioEnabled;
	bool musicEnabled;

};

}
