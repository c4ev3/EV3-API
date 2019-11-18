#ifndef EV3_API_EV3_COLOR_PRIVATE_H
#define EV3_API_EV3_COLOR_PRIVATE_H

#include "../../include/ev3_sensors/ev3_sensors.h"


bool initEV3ColorSensor(int port);

void exitEV3ColorSensor(int port);

int getEV3ColorSensorModeFromReadingLightMode(EV3ColorLightReadingMode);

int readEV3ColorSensorRawValue(int port, int mode);

void setEV3ColorSensorMode(int port, int mode);

#endif //EV3_API_EV3_COLOR_H
