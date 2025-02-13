This is a fork and a port of a game project from [codeberg](https://codeberg.org/yombo/smadness/src/branch/main/experiments/StarshipMadness)

# Spaceship Madness game ported to Nintendo 64

    Copyright © 2024 Yomboprime GPL 2.0 codeberg.org/yombo

    See licenses/LICENSE.GPL.txt for the full license.

    Copyright © 2024 KallistiOS framework license: KOSLicense (2-clause BSD based)

    See licenses/LICENSE.KOS.txt for the full license and licenses/KOS_AUTHORS.txt
    for the copyright holders list.

![boxart](https://github.com/user-attachments/assets/1bad9ee6-bf00-414a-81a3-b2d253abf95c)

# Game authors

    Music: airsynth

    Concept, code: yomboprime

    Ideas, testing, donated hardware and motivation: Carles


# Credits

    KallistiOS: KallistiOS authors (see licenses/KOS_AUTHORS.txt)

    See docs/List of asset licenses.ods for full asset list licenses.

    Ship model: Gundy

    Sound effects pack: phoenix1291

    Font: Andrew Young (Creative commons 4.0)

    Ported to the Nintendo 64 by SpookyIluha

# Porting

This game is written in C++ with usage of KOS on Dreamcast, std library and posix io, the 3D rendering is done with OpenGL 1.1.
There were some changes needed to bring this game over to the N64, namely replacing KOS calls with Libdragon equivalents and the font system is replaced with Libdragon's equivalent (namely to avoid large textures that would not fit TMEM). Other than that there are small tweaks to the rendering values and rendering equasion so that the game would fit into the N64's restrictions.

# Running

Tested on Ares and real hardware. ```.z64``` image can be obtained in releases.

![screenshot1](https://github.com/user-attachments/assets/6f814248-c631-436e-a257-8ef0542168d2)
![screenshot2](https://github.com/user-attachments/assets/83bb90aa-d378-45a4-bf83-d04d825e7949)



