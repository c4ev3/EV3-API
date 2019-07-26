#include "ev3_input.h"
#include "ev3_input_uart.h"
#include "ev3_input_analog.h"
#include "ev3_input_iic.h"
#include "../../copied/lms2012/ev3_uart.h"


DEVCON devCon;

static bool ev3SensorsInitialized = false;

bool initInput() {
    if (ev3SensorsInitialized) {
        return false;
    }
    bool analogInitialized = initEV3AnalogInput();
    bool uartInitialized = initEV3UARTInput();
    bool iicInitialized = initEV3IICnput();
    if (!analogInitialized || !uartInitialized || !iicInitialized) {
        // TODO: Close initialized resources
        return false;
    }
    ev3SensorsInitialized = true;
    return true;
}

void exitInput(){
    exitEV3AnalogInput();
    exitEV3UARTInput();
    exitEV3IICInput();
}
