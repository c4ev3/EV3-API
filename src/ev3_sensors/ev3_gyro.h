#ifndef EV3_API_EV3_GYRO_H
#define EV3_API_EV3_GYRO_H

#include "../../copied/lms2012/ev3_uart.h"
#include "sensors.h"

extern SensorHandler * EV3Gyro;

typedef enum EV3GyroMode {
    EV3GyroAngle,
    EV3GyroRate
} EV3GyroMode;

bool initEV3GyroSensor (int port);

int ReadEV3GyroSensor(int port, EV3GyroMode mode);

// TODO: To remove
extern DEVCON devCon;

void ResetEV3GyroSensor(int port);

/* Utility functions */
int getAngleFromAngleAndRate (uint64_t angleAndRate);

int getRateFromAngleAndRate (uint64_t angleAndRate);

void exitEV3GyroSensor (int port);

#endif //EV3_API_EV3_GYRO_H
