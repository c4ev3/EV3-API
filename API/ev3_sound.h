/** \file ev3_sound.h
 * \brief Functions and constants for controlling EV3 sounds
 *
 * ev3_sound.h contains declarations for the EV3 C API sound functions.
 *
 * License:
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
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

void PlayFileEx(char* pFileName, byte volume, bool loop);

#define PlayFile(_f) PlayFileEx((_f), 100, FALSE)

void PlayToneEx(unsigned short frequency, unsigned short duration, byte volume);

#define PlayTone(_f, _d) PlayToneEx((_f), (_d), 100)

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
void PlaySound(byte aCode);

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
