#ifndef EV3_API_EV3_TOUCH_SENSOR_H
#define EV3_API_EV3_TOUCH_SENSOR_H

#include <stdbool.h>
#include "ev3_sensors.h"

#define EV3_TOUCH_SENSOR_TOUCH_MODE 0

extern SensorHandler * EV3Touch;

/**
 * Read the state of the touch sensor
 * @param sensorPort port to which the sensor is connected
 * @return true if pressed, false if not
 */
bool ReadEV3TouchSensor(int sensorPort);


#endif //EV3_API_EV3_TOUCH_SENSOR_H
