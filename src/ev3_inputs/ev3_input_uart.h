#ifndef EV3_API_EV3_INPUT_UART_H
#define EV3_API_EV3_INPUT_UART_H

#include <stdbool.h>
#include "../../copied/lms2012/ev3_analog.h"
#include "../../copied/lms2012/ev3_basictypes.h"

bool initEV3UARTInput(ANALOG * analogSensors);

bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode);

int readFromUART(int port, DATA8 * buffer, int length);

void exitEV3UARTInput();

/* Utility functions */
int getUARTStatus(int port);
int waitNonZeroUARTStatusAndGet(int port);
void clearUARTChanged (int port);


#endif //EV3_API_EV3_INPUT_UART_H
