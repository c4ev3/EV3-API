#ifndef EV3_API_EV3_ULTRASONIC_H
#define EV3_API_EV3_ULTRASONIC_H

#include <stdbool.h>
#include "ev3_sensors.h"

#define EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE  0
#define EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE  1
#define EV3_ULTRASONIC_SENSOR_LISTEN_MODE       2

extern SensorHandler * EV3Ultrasonic;

typedef enum EV3UltrasonicDistanceUnit {
    EV3_ULTRASONIC_CM,
    EV3_ULTRASONIC_MM,
    EV3_ULTRASONIC_IN
} EV3UltrasonicDistanceUnit;

/**
 * Returns the measured distance by the sensor
 * @param port port to which the sensor is connected
 * @param mode wheter to read the distance in cm, mm or inches
 * @return measured value or -1 in case of error
 */
int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode);

/**
 * Returns true if sensor finds another ultrasonic sensor reading in distance mode
 * @param port port to which the sensor is connected
 * @return true if there exists another ultrasonic sensor reading in distance mode
 */
bool ReadEV3UltrasonicSensorListen(int port);

#endif //EV3_API_EV3_ULTRASONIC_H
