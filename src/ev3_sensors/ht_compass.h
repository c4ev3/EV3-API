#ifndef EV3_API_HT_COMPASS_H
#define EV3_API_HT_COMPASS_H

#include "ev3_sensors.h"

#define HT_COMPASS_SENSOR_DEFAULT_MODE 0

extern SensorHandler * HTCompass;

typedef enum HTCompassReadingMode {
    HTCompassCompass,
    HTCompassAngle
} HTCompassReadingMode;

bool initHTCompassSensor(int port);

int ReadHTCompassSensor(int port, HTCompassReadingMode mode);

void StartHTCompassCalibration(int port);

void StopHTCompassCalibration(int port);

void exitHTCompassSensor(int port);

#endif //EV3_API_HT_COMPASS_H
