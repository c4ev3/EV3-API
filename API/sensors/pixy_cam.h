#ifndef EV3_API_PIXY_CAM_PRIVATE_H
#define EV3_API_PIXY_CAM_PRIVATE_H

#include <stdbool.h>
#include "ev3_sensors.h"

extern SensorHandler * PixyCam;


bool initPixyCamSensor(int port);

#endif //EV3_API_PIXY_CAM_H
