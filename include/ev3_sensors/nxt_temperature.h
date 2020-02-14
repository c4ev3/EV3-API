#ifndef EV3_API_NXT_TEMPERATURE_H
#define EV3_API_NXT_TEMPERATURE_H

#include "ev3_sensors.h"

#define NXT_TEMPERATURE_SENSOR_DEFAULT_MODE 0


extern SensorHandler * NXTTemperature;

typedef enum NXTTemperatureUnit {
    NXT_TEMPERATURE_C,
    NXT_TEMPERATURE_F
} NXTTemperatureUnit;

int ReadNXTTemperatureSensor(int port, NXTTemperatureUnit unit);

#endif //EV3_API_NXT_TEMPERATURE_H
