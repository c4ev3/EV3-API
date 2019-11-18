#ifndef EV3_API_HT_IR_PRIVATE_H
#define EV3_API_HT_IR_PRIVATE_H

#include "../../include/ev3_sensors/ev3_ir.h"


bool initHTIrSensor(int port);

void exitHTIrSensor(int port);

int getHTIrRegisterForReadingMode(HTIrReadingMode mode);

#endif
