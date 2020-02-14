#ifndef EV3_API_NXT_SOUND_H
#define EV3_API_NXT_SOUND_H

#include "ev3_sensors.h"

extern SensorHandler * NXTSound;

typedef enum NXTSoundReadingMode {
    NXT_SOUND_DB,
    NXT_SOUND_DBA
} NXTSoundReadingMode;

int ReadNXTSoundSensor(int port, NXTSoundReadingMode mode);

#endif //EV3_API_NXT_SOUND_H
