#include "../inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "utility.h"

void setUARTSensorHandlerMode(SensorHandler *handler, int port, int type, int mode) {
    int previousMode = handler->currentSensorMode[port];
    setUARTSensorMode(port, type, mode);
    handler->currentSensorMode[port] = mode;
    if (previousMode != mode) {
        Wait(200);
    }
}
