#ifndef EV3_API_SENSORS_H
#define EV3_API_SENSORS_H


#include <stdbool.h>

typedef struct SensorHandler {
    bool (*Init)(int port);
    void (*Exit)(int port);
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

bool SetAllSensors (SensorHandler * port1, SensorHandler * port2, SensorHandler * port3, SensorHandler * port4);


#endif //EV3_API_SENSORS_H
