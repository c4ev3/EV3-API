#ifndef EV3_API_EV3_INPUT_ANALOG_H
#define EV3_API_EV3_INPUT_ANALOG_H

#include <stdbool.h>
#include "contrib/lms2012/ev3_analog.h"
#include "contrib/lms2012/ev3_basictypes.h"

ANALOG * initEV3AnalogInput(void);

DATA16 readNewDumbSensor(int port);

DATA16 readOldDumbSensor(int port);

void exitEV3AnalogInput(void);

#endif //EV3_API_EV3_INPUT_ANALOG_H
