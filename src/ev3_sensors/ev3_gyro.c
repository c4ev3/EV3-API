#include "../ev3_inputs/ev3_input_uart.h"
#include "../../firmware_headers/lms2012/ev3_basictypes.h"
#include "../../include/ev3_wait.h"
#include "ev3_gyro.h"

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
        value = ((value & 0x7FFF) - 0x7FFF);
    }
    return value;
}

// TODO: Apply the reset procedure used in pxt, which can handle the drift via software correction
void ResetEV3GyroSensor(int port) {
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_RATE_MODE);
    Wait(200);
    setEV3GyroSensorMode(port, EV3_GYRO_SENSOR_RATE_MODE);
    Wait(500);
    setEV3GyroSoftwareReset(port);
}

void setEV3GyroSensorMode(int port, int mode) {
    EV3Gyro->currentSensorMode[port] = mode;
    setUARTSensorMode(port, EV3_GYRO_SENSOR_TYPE, mode);
}

void setEV3GyroSoftwareReset(int port) {
    ev3GyroSoftwareResetOffset[port] = 0;
    ev3GyroSoftwareResetOffset[port] = ReadEV3GyroSensor(port, EV3GyroAngle);
}

void exitEV3GyroSensor (int port) {

}
