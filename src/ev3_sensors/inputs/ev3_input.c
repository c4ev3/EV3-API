#include "ev3_input.h"
#include "ev3_input_uart.h"
#include "ev3_input_analog.h"

bool initialized = false;

bool initInput() {
    bool analogInitialized = initEV3AnalogInput();
    bool uartInitialized = initEV3UARTInput();
    if (!analogInitialized || !uartInitialized) {
        // TODO: Close initialized resources
        return false;
    }
    initialized = true;
    return true;
}

void exitInput(){
    exitEV3AnalogInput();
    exitEV3UARTInput();
}
