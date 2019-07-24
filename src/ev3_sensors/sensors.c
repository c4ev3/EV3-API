#include <stddef.h>
#include "../ev3_constants.h"
#include "sensors.h"

bool SetAllSensors (SensorHandler * port1, SensorHandler * port2, SensorHandler * port3, SensorHandler * port4) {
    SensorHandler * handlers[NUM_INPUTS] = {port1, port2, port3, port4};
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        if (handlers[i] != NULL) {
            bool res = handlers[i]->Init(i);
            if (!res) {
                // TODO: Deallocate
                return false;
            }
        }
    }
}

