#ifndef EV3_API_EV3_TOUCH_SENSOR_H
#define EV3_API_EV3_TOUCH_SENSOR_H

#include <stdbool.h>
#include "ev3_sensors.h"

#define EV3_TOUCH_SENSOR_TOUCH_MODE 0

extern SensorHandler * EV3Touch;

bool ReadEV3TouchSensor(int sensorPort);


#endif //EV3_API_EV3_TOUCH_SENSOR_H
