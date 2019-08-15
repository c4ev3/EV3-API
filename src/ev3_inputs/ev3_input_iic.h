#ifndef EV3_API_EV3_INPUT_IIC_H
#define EV3_API_EV3_INPUT_IIC_H

#include <stdbool.h>
#include "../../copied/lms2012/ev3_basictypes.h"

bool initEV3IICInput();

bool initIICPort(int port, int deviceAddress);

int readFromIIC(int port, int registerAddress, DATA8 * buffer, int length);

int readFromIICSharedMemory(int port, DATA8 * buffer, int length);

void writeToIIC(int port, DATA8 *toWrite, int toWriteLength, int repeatTimes, int repeatInterval, int responseLength);

void startPollingFromIIC(int port, int registerAddress, int pollDelay);

void exitEV3IICInput();

#endif //EV3_API_EV3_INPUT_IIC_H
