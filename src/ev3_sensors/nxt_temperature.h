#ifndef EV3_API_NXT_TEMPERATURE_H
#define EV3_API_NXT_TEMPERATURE_H

#include "ev3_sensors.h"

extern SensorHandler * NXTTemperature;

typedef enum NXTTemperatureUnit {
    C,
    F
} NXTTemperatureUnit;

bool initNXTTemperatureSensor(int port);

int ReadNXTTemperatureSensor(int port, NXTTemperatureUnit unit);

void exitNXTTemperatureSensor(int port);

#endif //EV3_API_NXT_TEMPERATURE_H
