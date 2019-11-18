#ifndef EV3_API_BACK_COMPATIBILITY_PRIVATE_H
#define EV3_API_BACK_COMPATIBILITY_PRIVATE_H

#include "../../include/ev3_sensors/back_compatibility.h"

void initializeAllSensors();

void initializeBackCompatibilityIfNeeded();

int readEV3IrSensorSeekForBackCompatibility(int port);

#endif
