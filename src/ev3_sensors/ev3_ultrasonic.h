#ifndef EV3_API_EV3_ULTRASONIC_PRIVATE_H
#define EV3_API_EV3_ULTRASONIC_PRIVATE_H

#include "../../include/ev3_sensors/ev3_ultrasonic.h"


bool initEV3UltrasonicSensor (int port);

void exitEV3UltrasonicSensor (int port);

void setEV3UltrasonicSensorMode(int port, int mode);

int getEV3UltrasonicSensorModeFromDistanceUnit(EV3UltrasonicDistanceUnit mode);

#endif
