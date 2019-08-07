#ifndef EV3_API_EV3_INPUT_IIC_H
#define EV3_API_EV3_INPUT_IIC_H

#include <stdbool.h>
#include "../../copied/lms2012/ev3_basictypes.h"

bool initEV3IICnput();


int readFromIIC(int sensorPort, DATA8 * buffer, int length);

//void writeIicRequestUsingIoctl(int sensorPort, int address, DATA8 toWrite[], int toWriteLength, int repeatTimes, int repeatInterval,  int responseLength);


void exitEV3IICInput();

#endif //EV3_API_EV3_INPUT_IIC_H
