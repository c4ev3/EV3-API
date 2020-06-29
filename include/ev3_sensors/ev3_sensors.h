/** \file ev3_sensors.h
 * \brief Imports all the c4ev3 supported sensors.
 *
 */

#ifndef EV3_API_EV3_SENSORS_H
#define EV3_API_EV3_SENSORS_H

#include <stdbool.h>
#include "ev3_constants.h"

#define NONE_MODE -1

struct SensorHandler;

typedef struct SensorHandler SensorHandler;

/**
 * Initializes the specified sensor connected to the specified port
 * @param port
 * @param sensor
 * @return true if the initialization was successful
 */
bool SetSensor (int port, SensorHandler * sensor);

/**
 * Returns the sensor handler initialized for the specified port
 * @param port port
 * @return Sensor handler or null
 */
SensorHandler * GetSensor (int port);

/**
 * Initializes the specified sensor handlers for the 4 different robot ports.
 * Specify null to not initialize any sensor for a specific port.
 * If one sensor initialization fails, the previous initialized sensors are
 * de-initialized and false is returned.
 * @param port1
 * @param port2
 * @param port3
 * @param port4
 * @return true if the initialization was successful
 */
bool SetAllSensors (SensorHandler * port1, SensorHandler * port2, SensorHandler * port3, SensorHandler * port4);

#include "ev3_touch.h"
#include "ev3_color.h"
#include "ev3_ir.h"
#include "ht_ir_v2.h"
#include "ev3_ultrasonic.h"
#include "ev3_gyro.h"
#include "nxt_temperature.h"
#include "nxt_sound.h"
#include "ht_compass.h"
#include "ht_color.h"
#include "pixy_cam.h"
#include "ev3_sensors/c4ev3_compatibility.h"

#endif //EV3_API_EV3_SENSORS_H
