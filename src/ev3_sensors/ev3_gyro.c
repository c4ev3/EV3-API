#include "../ev3_inputs/ev3_input_uart.h"
#include "../ev3_time.h"
#include "../ev3_sensor.h"
#include "../../copied/lms2012/ev3_basictypes.h"
#include "ev3_gyro.h"

#define EV3_GYRO_SENSOR_TYPE                32
#define EV3_GYRO_SENSOR_ANGLE_MODE          0
#define EV3_GYRO_SENSOR_RATE_MODE           1
#define EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE 3
#define EV3_GYRO_SENSOR_DEFAULT_MODE        EV3_GYRO_SENSOR_ANGLE_AND_RATE_MODE

SensorHandler * EV3Gyro = &(SensorHandler){
    .Init = initEV3GyroSensor,
    .Exit = exitEV3GyroSensor
};

bool initEV3GyroSensor (int port) {
    LcdTextf(1, 0, 10, "Initialization");
    LcdTextf(1, 0, 20, "Gyro at %d", port + 1);
    setUARTSensorModeIfNeeded(port, EV3_GYRO_SENSOR_TYPE, EV3_GYRO_SENSOR_DEFAULT_MODE);
}

// TODO: Rate mode doesn't work

int ReadEV3GyroSensor(int port, EV3GyroMode mode) {
    setUARTSensorModeIfNeeded(port, EV3_GYRO_SENSOR_TYPE, EV3_GYRO_SENSOR_DEFAULT_MODE);

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
    LcdTextf(1, 0, 10, "Reset");
    LcdTextf(1, 0, 20, "Gyro at %d", port + 1);
    setUARTSensorMode(port, EV3_GYRO_SENSOR_TYPE, EV3_GYRO_SENSOR_RATE_MODE);
    Wait(200);
    setUARTSensorMode(port, EV3_GYRO_SENSOR_TYPE, EV3_GYRO_SENSOR_DEFAULT_MODE); // sleep 1
    Wait(2000);
}

void exitEV3GyroSensor (int port) {

}
