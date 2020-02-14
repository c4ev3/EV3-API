#ifndef EV3_API_PIXY_CAM_H
#define EV3_API_PIXY_CAM_H

#include <stdbool.h>
#include "ev3_sensors.h"

extern SensorHandler * PixyCam;

typedef struct PixyRectangle {
    int signature;
    int x, y;
    int width, height;
    int angle;
} PixyRectangle;

/**
 * Reads the coordinates, size, signature and angle of the biggest blob found
 * by the sensor.
 * @param port port to which the sensor is connected to
 * @param result struct to which the result will be written  (output variable)
 * @return -1 if there was an error
 */
int ReadPixyCamSensorBiggestBlobRectangle(int port, PixyRectangle* result);

#endif //EV3_API_PIXY_CAM_H
