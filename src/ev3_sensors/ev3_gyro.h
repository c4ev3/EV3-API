#ifndef EV3_API_EV3_GYRO_H
#define EV3_API_EV3_GYRO_H

#include "../../copied/lms2012/ev3_uart.h"

typedef enum EV3GyroMode {
    EV3GyroAngle,
    EV3GyroRate
} EV3GyroMode;

int ReadEV3GyroSensor(int port, EV3GyroMode mode);

extern DEVCON devCon;

void ResetEV3GyroSensor(int port);

/* Utility functions */
int getAngleFromAngleAndRate (uint64_t angleAndRate);

int getRateFromAngleAndRate (uint64_t angleAndRate);

#endif //EV3_API_EV3_GYRO_H
