#ifndef EV3_API_EV3_SENSORS_H
#define EV3_API_EV3_SENSORS_H

#include <stdbool.h>
#include "../ev3_constants.h"

#define NONE_MODE -1

typedef struct SensorHandler {
    bool (*Init)(int port);
    void (*Exit)(int port);

    int currentSensorMode[NUM_INPUTS];
} SensorHandler;

#include "ev3_touch.h"
#include "ev3_color.h"
#include "ev3_ir.h"
#include "ht_ir.h"
#include "ev3_ultrasonic.h"
#include "ev3_gyro.h"
#include "nxt_temperature.h"
#include "nxt_sound.h"
#include "ht_compass.h"

bool SetSensor (int port, SensorHandler * sensor);

SensorHandler * GetSensor (int port);

bool SetAllSensors (SensorHandler * port1, SensorHandler * port2, SensorHandler * port3, SensorHandler * port4);

#endif //EV3_API_EV3_SENSORS_H
