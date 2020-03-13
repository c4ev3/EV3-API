#ifndef EV3_API_EV3_SENSORS_PRIVATE_H
#define EV3_API_EV3_SENSORS_PRIVATE_H

#include <stdbool.h>
#include <stddef.h>
#include "ev3_constants.h"
#include <ev3_sensors/ev3_sensors.h>
#include "ev3_sensors/c4ev3_compatibility.h"

struct SensorHandler {

    /**
     * Initializes the sensor handler for the specified port.
     * If the initialization fails, the function should return false and clean
     * up resources (in order to allow a future call to Init)
     * @param port port to which the sensor is connected
     * @return true if the initialization was successful
     */
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
};

bool SensorsInit (void);

void SensorsExit (void);

/**
 * Calls Exit on the sensor handler of the sensors connected to port which index
 * is less then the specified port.
 * @param sensorHandlers
 * @param lastPortExcluded
 */
void exitSensorsUntilPort(SensorHandler * sensorHandlers[], int lastPortExcluded);

#endif
