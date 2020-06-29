#include "ev3_sensors.h"

#ifndef EV3_API_SENSORS_UTILITY_PRIVATE_H
#define EV3_API_SENSORS_UTILITY_PRIVATE_H

/**
 * Changes the mode of the sensor, calling the appropriate function of the UART
 * module, and updates the structure of the sensor handler.
 * NOTE: a delay is waited when the mode is changed
 * @param handler Sensor handler to which change the mode
 * @param port The port to which the sensor is connected
 * @param type Type of UART sensor, as defined in the lms2012 firmware
 * @param mode Mode of the sensor
 */
void setUARTSensorHandlerMode(SensorHandler *handler, int port, int type, int mode);

#endif
