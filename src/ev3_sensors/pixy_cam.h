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

bool initPixyCamSensor(int port);

PixyRectangle ReadPixyCamSensorBiggestBlobRectangle(int port);

void exitPixyCamSensor(int port);

#endif //EV3_API_PIXY_CAM_H
