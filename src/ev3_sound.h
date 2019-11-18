#ifdef __cplusplus
extern "C" {
#endif

#ifndef EV3_SOUND_PRIVATE_H
#define EV3_SOUND_PRIVATE_H

#include "../include/ev3_wait.h"
#include "../include/ev3_sound.h"


bool SoundInit();
bool SoundOpen();
bool SoundClose();
bool SoundExit();
bool SoundInitialized();

#endif

#ifdef __cplusplus
}
#endif
