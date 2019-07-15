#ifndef EV3_API_EV3_ULTRASONIC_H
#define EV3_API_EV3_ULTRASONIC_H

#include <stdbool.h>

typedef enum EV3UltrasonicDistanceUnit {
    CM = US_DIST_CM,
    MM = US_DIST_MM,
    IN = US_DIST_IN
} EV3UltrasonicDistanceUnit;

int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode);

bool ReadEV3UltrasonicSensorListen(int port);

#endif //EV3_API_EV3_ULTRASONIC_H
