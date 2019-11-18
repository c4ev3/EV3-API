#ifndef EV3_API_EV3_GYRO_H
#define EV3_API_EV3_GYRO_H

#include "ev3_sensors.h"

#define EV3_GYRO_SENSOR_ANGLE_MODE          0
#define EV3_GYRO_SENSOR_RATE_MODE           1
#define EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE 3

extern SensorHandler *EV3Gyro;

typedef enum EV3GyroReadingMode {
    EV3GyroAngle,
    EV3GyroRate
} EV3GyroReadingMode;


int ReadEV3GyroSensor(int port, EV3GyroReadingMode mode);

void ResetEV3GyroSensor(int port);


#endif //EV3_API_EV3_GYRO_H
