#ifndef EV3_API_HT_COMPASS_H
#define EV3_API_HT_COMPASS_H

#include "ev3_sensors.h"

extern SensorHandler * HTCompass;

typedef enum HTCompassMode {
    HTCompassCompass,
    HTCompassAngle
} HTCompassMode;

bool initHTCompassSensor(int port);

int ReadHTCompassSensor(int port, HTCompassMode mode);

void StartHTCompassCalibration(int port);

void StopHTCompassCalibration(int port);

void exitHTCompassSensor(int port);

#endif //EV3_API_HT_COMPASS_H
