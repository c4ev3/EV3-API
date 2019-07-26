#ifndef EV3_API_EV3_ULTRASONIC_H
#define EV3_API_EV3_ULTRASONIC_H

#include <stdbool.h>
#include "sensors.h"

extern SensorHandler * EV3Ultrasonic;

typedef enum EV3UltrasonicDistanceUnit {
    CM = US_DIST_CM,
    MM = US_DIST_MM,
    IN = US_DIST_IN
} EV3UltrasonicDistanceUnit;


bool initEV3UltrasonicSensor (int port);

int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode);

bool ReadEV3UltrasonicSensorListen(int port);


void exitEV3UltrasonicSensor (int port);

/* Utility functions */
int getEV3UltrasonicSensorDistanceModeConstant(EV3UltrasonicDistanceUnit mode);

#endif //EV3_API_EV3_ULTRASONIC_H
