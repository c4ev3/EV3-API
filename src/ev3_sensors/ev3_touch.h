#ifndef EV3_API_EV3_TOUCH_SENSOR_PRIVATE_H
#define EV3_API_EV3_TOUCH_SENSOR_PRIVATE_H

#include <stdbool.h>
#include "../../include/ev3_sensors/ev3_touch.h"

bool initEV3Touch(int port);

void exitEV3Touch(int port);

void setEV3UltrasonicSensorMode(int port, int mode);

#endif
