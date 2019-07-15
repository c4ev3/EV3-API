#ifndef EV3_API_NXT_SOUND_H
#define EV3_API_NXT_SOUND_H

#include "../ev3_sensor.h"

typedef enum NXTSoundSensorMode {
    DB = NXT_SOUND_DB,
    DBA = NXT_SOUND_DBA
} NXTSoundSensorMode;

int ReadNXTSoundSensor(int port, NXTSoundSensorMode mode);

#endif //EV3_API_NXT_SOUND_H
