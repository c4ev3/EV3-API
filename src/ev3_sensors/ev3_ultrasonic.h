#ifndef EV3_API_EV3_ULTRASONIC_H
#define EV3_API_EV3_ULTRASONIC_H

#include <stdbool.h>
#include "ev3_sensors.h"

#define EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE  0
#define EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE  1
#define EV3_ULTRASONIC_SENSOR_LISTEN_MODE       2

extern SensorHandler * EV3Ultrasonic;

typedef enum EV3UltrasonicDistanceUnit {
    CM,
    MM,
    IN
} EV3UltrasonicDistanceUnit;


bool initEV3UltrasonicSensor (int port);

int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode);

bool ReadEV3UltrasonicSensorListen(int port);

void exitEV3UltrasonicSensor (int port);

/* Utility functions */
int getEV3UltrasonicSensorModeFromDistanceUnit(EV3UltrasonicDistanceUnit mode);

#endif //EV3_API_EV3_ULTRASONIC_H
