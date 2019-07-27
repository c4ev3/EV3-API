#ifndef EV3_API_NXT_SOUND_H
#define EV3_API_NXT_SOUND_H

#include "ev3_sensors.h"

extern SensorHandler * NXTSound;

// TODO: Mode not implemented yet
typedef enum NXTSoundSensorMode {
    DB,
    DBA
} NXTSoundSensorMode;

bool initNXTSoundSensor (int port);

int ReadNXTSoundSensor(int port, NXTSoundSensorMode mode);

void exitNXTSoundSensor(int port);

#endif //EV3_API_NXT_SOUND_H
