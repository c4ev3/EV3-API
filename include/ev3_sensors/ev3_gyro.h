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

/**
 * Read angle or rate from gyro sensor
 * @param port port to which the sensor is connected
 * @param mode whether to read the angle or the rate
 * @return angle or rate of the gyro sensor
 */
int ReadEV3GyroSensor(int port, EV3GyroReadingMode mode);

/**
 * Resets the gyro sensor connected to the specified port
 * @param port port to which the sensor is connected
 */
void ResetEV3GyroSensor(int port);


#endif //EV3_API_EV3_GYRO_H
