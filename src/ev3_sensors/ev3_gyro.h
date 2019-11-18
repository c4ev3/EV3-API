#ifndef EV3_API_EV3_GYRO_PRIVATE_H
#define EV3_API_EV3_GYRO_PRIVATE_H

#include "../../include/ev3_sensors/ev3_gyro.h"


bool initEV3GyroSensor(int port);

void exitEV3GyroSensor(int port);

int getAngleFromAngleAndRate(uint64_t angleAndRate);

int getRateFromAngleAndRate(uint64_t angleAndRate);

int handleEV3GyroNegativeValue(int value);

void setEV3GyroSoftwareReset(int port);

void setEV3GyroSensorMode(int port, int mode);


#endif //EV3_API_EV3_GYRO_H
