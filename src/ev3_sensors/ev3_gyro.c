#include "../ev3_wait.h"
#include "../ev3_sensor.h"
#include "../../copied/lms2012/ev3_basictypes.h"
#include "ev3_gyro.h"

#ifndef GYRO_TYPE
#define GYRO_TYPE 				32
#define GYRO_RATE_MODE 			1 // rate
#define GYRO_ANG_AND_RATE_MODE 	3 // both
#endif

// TODO: Rate mode doesn't work

int ReadEV3GyroSensor(int port, EV3GyroMode mode) {
    SwitchSensorToModeIfNeeded(port, GYRO_ANG_AND_RATE_MODE);

    uint64_t angleAndRate;
    readFromUART(port, &angleAndRate, 4);

    if (mode == EV3GyroAngle) {
        return getAngleFromAngleAndRate(angleAndRate);
    } else {
        return getRateFromAngleAndRate(angleAndRate);
    }
}

int getAngleFromAngleAndRate (uint64_t angleAndRate) {
    int temp = angleAndRate & 0xFFFF;

    // handles negative values
    if(temp & 0x8000) {
        temp = ((temp&0x7FFF) - 0x7FFF);
    }
    return temp;
}

int getRateFromAngleAndRate (uint64_t angleAndRate) {
    int temp = (angleAndRate >> 16) & 0xFFFF;

    // handles negative values
    if(temp & 0x8000) {
        temp = ((temp&0x7FFF) - 0x7FFF);
    }
    return temp;
}

void ResetEV3GyroSensor(int port) {
    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = GYRO_TYPE;
    devCon.Mode[port] = GYRO_RATE_MODE;
    applySensorMode();
    Wait(200);

    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = GYRO_TYPE;
    devCon.Mode[port] = GYRO_ANG_AND_RATE_MODE;
    applySensorMode();
    Wait(200);
}
