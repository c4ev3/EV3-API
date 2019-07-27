#ifndef EV3_API_EV3_INPUT_ANALOG_H
#define EV3_API_EV3_INPUT_ANALOG_H

#include <stdbool.h>
#include "../../copied/lms2012/ev3_analog.h"
#include "../../copied/lms2012/ev3_basictypes.h"

ANALOG * initEV3AnalogInput();

DATA16 readNewDumbSensor(int port);

DATA16 readOldDumbSensor(int port);

void exitEV3AnalogInput();

#endif //EV3_API_EV3_INPUT_ANALOG_H
