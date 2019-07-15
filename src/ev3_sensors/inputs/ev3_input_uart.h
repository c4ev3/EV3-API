#ifndef EV3_API_EV3_INPUT_UART_H
#define EV3_API_EV3_INPUT_UART_H

#include <stdbool.h>
#include "../../../copied/lms2012/ev3_basictypes.h"

bool initEV3UARTInput();

int readFromUART(int sensorPort, DATA8 * buffer, int length);

void exitEV3UARTInput();

#endif //EV3_API_EV3_INPUT_UART_H
