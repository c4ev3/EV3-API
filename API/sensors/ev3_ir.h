#ifndef EV3_API_EV3_IR_PRIVATE_H
#define EV3_API_EV3_IR_PRIVATE_H

#include "ev3_ir.h"

bool initEV3IrSensor (int port);

void exitEV3IrSensor (int port);

void setEV3IrSensorMode(int port, int mode);


#endif //EV3_API_EV3_IR_H
