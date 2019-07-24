#ifndef EV3_API_EV3_TOUCH_SENSOR_H
#define EV3_API_EV3_TOUCH_SENSOR_H

#include <stdbool.h>
#include "sensors.h"

extern SensorHandler * EV3Touch;

bool initEV3Touch(int port);

bool ReadEV3TouchSensor(int sensorPort);

void exitEV3Touch(int port);

#endif //EV3_API_EV3_TOUCH_SENSOR_H
