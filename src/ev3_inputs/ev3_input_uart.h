#ifndef EV3_API_EV3_INPUT_UART_H
#define EV3_API_EV3_INPUT_UART_H

#include <stdbool.h>
#include "../../copied/lms2012/ev3_basictypes.h"

bool initEV3UARTInput();

bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode);

bool setUARTSensorModeIfNeeded (int port, DATA8 sensorType, DATA8 sensorMode);

int readFromUART(int port, DATA8 * buffer, int length);

void exitEV3UARTInput();

/* Utility functions */
int getUARTStatus(int port);
int waitNonZeroUARTStatusAndGet(int port);
void clearUARTChanged (int port);
bool isUARTSensorModeDifferent(int port, DATA8 sensorType, DATA8 sensorMode);

#endif //EV3_API_EV3_INPUT_UART_H
