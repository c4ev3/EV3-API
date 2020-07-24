/** \file ev3_sound.h
 * \brief Functions and constants for controlling EV3 sounds
 *
 * ev3_sound.h contains declarations for the EV3 C API sound functions.
 *
 * License:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 *
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author John Hansen (bricxcc_at_comcast.net)
 * \date 2013-06-20
 * \version 1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_sound_h
#define ev3_sound_h

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

#include "ev3_command.h"
#include "ev3_constants.h"

typedef struct
{
	unsigned short Frequency;
	unsigned short Duration;
} Tone;

bool SoundInit();
bool SoundOpen();
bool SoundClose();
bool SoundExit();
bool SoundInitialized();

void PlayFileEx(char* pFileName, uint8_t volume, bool loop);

#define PlayFile(_f) PlayFileEx((_f), 100, false)

void PlayToneEx(unsigned short frequency, unsigned short duration, uint8_t volume);

#define PlayTone(_f, _d) PlayToneEx((_f), (_d), 100)

void PlaySoundEx(uint8_t aCode, uint8_t volume);

/**
 * Play a system sound.
 * Play a sound that mimics the RCX system sounds using one of the
 * \ref RCXSoundConstants.
 * <TABLE BORDER=1>
 * <TR><TH>aCode</TH><TH>Resulting Sound</TH></TR>
 * <TR><TD>\ref SOUND_CLICK</TD><TD>key click sound</TD></TR>
 * <TR><TD>\ref SOUND_DOUBLE_BEEP</TD><TD>double beep</TD></TR>
 * <TR><TD>\ref SOUND_DOWN</TD><TD>sweep down</TD></TR>
 * <TR><TD>\ref SOUND_UP</TD><TD>sweep up</TD></TR>
 * <TR><TD>\ref SOUND_LOW_BEEP</TD><TD>error sound</TD></TR>
 * <TR><TD>\ref SOUND_FAST_UP</TD><TD>fast sweep up</TD></TR>
 * </TABLE>
 * \param aCode The system sound to play.  See \ref RCXSoundConstants.
 */
#define PlaySound(aCode) PlaySoundEx((aCode), 100)

/**
 * Play multiple tones.
 * Play a series of tones contained in the tones array.  Each element
 * in the array is an instance of the \ref Tone structure, containing
 * a frequency and a duration.
 *
 * \param tones The array of tones to play.
 * \param size The number of tones to play.
 */
void PlayTonesEx(Tone tones[], size_t size);

#define PlayTones(_tones) PlayTonesEx(_tones, sizeof(_tones)/sizeof(_tones[0]))

int SoundState();

void StopSound();

bool SoundTest();

void SoundReady();

void MuteSound();

void UnmuteSound();

void ClearSound();

#endif // ev3_sound_h

#ifdef __cplusplus
}
#endif
