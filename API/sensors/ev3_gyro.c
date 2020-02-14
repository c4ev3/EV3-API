#include "inputs/ev3_input_uart.h"
#include "ev3_sensors.h"
#include "contrib/lms2012/ev3_basictypes.h"
#include "ev3_command.h"
#include "ev3_gyro.h"
#include "utility.h"

#define EV3_GYRO_SENSOR_TYPE                32
#define EV3_GYRO_SENSOR_DEFAULT_MODE        EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE

int ev3GyroSoftwareResetOffset[] = {0, 0, 0, 0};

SensorHandler * EV3Gyro = &(SensorHandler){
    .Init = initEV3GyroSensor,
    .Exit = exitEV3GyroSensor,
    .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initEV3GyroSensor (int port) {
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_DEFAULT_MODE);
    return true;
}


int ReadEV3GyroSensor(int port, EV3GyroReadingMode mode) {
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_DEFAULT_MODE);

    uint64_t angleAndRate;
    readFromUART(port, (DATA8 *) &angleAndRate, 4);

    if (mode == EV3GyroAngle) {
        int angle = getAngleFromAngleAndRate(angleAndRate);
        return angle - ev3GyroSoftwareResetOffset[port];
    } else {
        return getRateFromAngleAndRate(angleAndRate);
    }
}

int getAngleFromAngleAndRate (uint64_t angleAndRate) {
    int value = angleAndRate & 0xFFFF;
    return handleEV3GyroNegativeValue(value);
}

int getRateFromAngleAndRate (uint64_t angleAndRate) {
    int value = (angleAndRate >> 16) & 0xFFFF;
    return handleEV3GyroNegativeValue(value);
}

int handleEV3GyroNegativeValue(int value) {
    if(value & 0x8000) {
        /**
         * - the if checks for the sign-bit
         * - the and-mask removes the sign bit - that gives us how much bigger is
         *   the actual value compared to the most negative value
         * - the subtraction just shifts the result so that it matches the
         *   previous value. In this case, a +1 is also added, presumably to
         *   prevent the confusing situation that the most negative value does
         *   not have a positive counterpart.
         *
         * Explanation provided by @JakubVanek, see https://github.com/c4ev3/ev3-api/issues/9
         */
        value = ((value & 0x7FFF) - 0x7FFF);
    }
    return value;
}

void ResetEV3GyroSensor(int port) {
#if EV3_GYRO_SENSOR_DEFAULT_MODE == EV3_GYRO_SENSOR_RATE_MODE
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_ANGLE_MODE);
#else
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_RATE_MODE);
#endif
    Wait(200);
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_DEFAULT_MODE);
    Wait(500);
    setEV3GyroSoftwareReset(port);
}

void setEV3GyroSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Gyro, port, EV3_GYRO_SENSOR_TYPE, mode);
}

void setEV3GyroSoftwareReset(int port) {
    ev3GyroSoftwareResetOffset[port] = 0;
    ev3GyroSoftwareResetOffset[port] = ReadEV3GyroSensor(port, EV3GyroAngle);
}

void exitEV3GyroSensor (int port) {

}
