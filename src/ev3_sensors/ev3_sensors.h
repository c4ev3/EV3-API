#ifndef EV3_API_EV3_SENSORS_H
#define EV3_API_EV3_SENSORS_H

#include <stdbool.h>
#include "../ev3_constants.h"
#include "back_compatibility.h"

#define NONE_MODE -1

typedef struct SensorHandler {
    bool (*Init)(int port);
    void (*Exit)(int port);

    /**
     * Contains a value for each sensor that represents the mode in which the sensor it's working.
     *
     * For example take the EV3 ultrasonic sensor:
     * the currentSensorMode could have three different values:
     * - listen mode
     * - distance mode mm
     * - distance mode in
     *
     * Reading the distance in cm is a different reading mode, because the conversion is done from code, it's not a value
     * that we receive from the sensor.
     */
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
#include "ht_color.h"
#include "pixy_cam.h"


bool SensorInit ();

bool SetSensor (int port, SensorHandler * sensor);

SensorHandler * GetSensor (int port);

bool SetAllSensors (SensorHandler * port1, SensorHandler * port2, SensorHandler * port3, SensorHandler * port4);

void SensorExit ();

#endif //EV3_API_EV3_SENSORS_H
