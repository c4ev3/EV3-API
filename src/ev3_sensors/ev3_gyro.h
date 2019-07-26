#ifndef EV3_API_EV3_GYRO_H
#define EV3_API_EV3_GYRO_H

#include "sensors.h"

extern SensorHandler * EV3Gyro;

typedef enum EV3GyroMode {
    EV3GyroAngle,
    EV3GyroRate
} EV3GyroMode;

bool initEV3GyroSensor (int port);

int ReadEV3GyroSensor(int port, EV3GyroMode mode);

void ResetEV3GyroSensor(int port);

/* Utility functions */
int getAngleFromAngleAndRate (uint64_t angleAndRate);

int getRateFromAngleAndRate (uint64_t angleAndRate);

void exitEV3GyroSensor (int port);

#endif //EV3_API_EV3_GYRO_H
