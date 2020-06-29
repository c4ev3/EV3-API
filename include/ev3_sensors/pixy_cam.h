#ifndef EV3_API_PIXY_CAM_H
#define EV3_API_PIXY_CAM_H

#include <stdbool.h>
#include "ev3_sensors.h"

#define PIXY_CAM_SENSOR_IIC_ADDRESS 0x01

#define PIXY_CAM_BIGGEST_BLOB_RECTANGLE_REGISTER    0x50
#define PIXY_CAM_BIGGEST_BLOB_ANGLE_REGISTER        0x60

#define PIXY_CAM_ANGLE_CONSTANT 1.40625

//! Untested.
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
