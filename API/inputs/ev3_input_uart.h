#ifndef EV3_API_EV3_INPUT_UART_H
#define EV3_API_EV3_INPUT_UART_H

#include <stdbool.h>
#include "contrib/lms2012/ev3_analog.h"
#include "contrib/lms2012/ev3_basictypes.h"

bool initEV3UARTInput(ANALOG * analogSensors);

/**
 * Sets the mode of a Lego UART sensor
 * @param port porto to which the sensor is connected
 * @param sensorType constant from firmware that identifies sensor type
 * @param sensorMode constant from firmware that identifies sensor mode
 * @return
 */
bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode);

/**
 * Read data form UART sensor
 * @param port port to which the sensor is connected
 * @param buffer buffer to which bytes will be written. The size of this buffer
 *      must be at least 'length' long
 * @param length how many bytes to read
 * @return -1 in case of error
 */
int readFromUART(int port, DATA8 * buffer, int length);


void exitEV3UARTInput(void);

/**
 * Wait until the device connected to this port is up and ready.
 * @param port Port that the device is plugged to.
 * @param portChanged Whether the kernel has signalled a UART_PORT_CHANGED
 *                    flag, therefore necessitating port reconfiuration.
 */
void waitUartReady(int port, bool *portChanged);

#endif //EV3_API_EV3_INPUT_UART_H
