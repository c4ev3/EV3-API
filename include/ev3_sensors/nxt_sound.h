#ifndef EV3_API_NXT_SOUND_H
#define EV3_API_NXT_SOUND_H

#include "ev3_sensors.h"

#define NXT_SOUND_SENSOR_TYPE       3
#define NXT_SOUND_SENSOR_DB_MODE    0
#define NXT_SOUND_SENSOR_DBA_MODE   1

#define NXT_SOUND_SENSOR_DEFAULT_MODE NXT_SOUND_SENSOR_DB_MODE

extern SensorHandler * NXTSound;

typedef enum NXTSoundReadingMode {
    NXT_SOUND_DB,
    NXT_SOUND_DBA
} NXTSoundReadingMode;

int ReadNXTSoundSensor(int port, NXTSoundReadingMode mode);

#endif //EV3_API_NXT_SOUND_H
