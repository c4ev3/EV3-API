#include "inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "ev3_sensors/ev3_ultrasonic.h"
#include "utility.h"
#include <limits.h>

//
// PRIVATE DECLARATIONS
//

static bool initEV3UltrasonicSensor (int port);

static void exitEV3UltrasonicSensor (int port);

static void setEV3UltrasonicSensorMode(int port, int mode);

static int distanceRead(int port, int mode, int divisor);

//
// GLOBAL DATA
//

SensorHandler * EV3Ultrasonic = &(SensorHandler){
        .Init = initEV3UltrasonicSensor,
        .Exit = exitEV3UltrasonicSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initEV3UltrasonicSensor (int port) {
    setEV3UltrasonicSensorMode(port, EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE);
    return true;
}


int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode) {
    switch (mode) {
        case EV3_ULTRASONIC_CM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE, 10);
        case EV3_ULTRASONIC_MM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE,  1);
        case EV3_ULTRASONIC_IN:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE,  1);
        default:
            return INT_MIN;
    }
}

int ReadEV3UltrasonicSensorSingleDistance(int port, EV3UltrasonicDistanceUnit mode) {
    switch (mode) {
        case EV3_ULTRASONIC_CM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_SINGLE_MM_MODE, 10);
        case EV3_ULTRASONIC_MM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_SINGLE_MM_MODE,  1);
        case EV3_ULTRASONIC_IN:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_SINGLE_IN_MODE,  1);
        default:
            return INT_MIN;
    }
}

int ReadEV3UltrasonicSensorDcDistance(int port, EV3UltrasonicDistanceUnit mode) {
    switch (mode) {
        case EV3_ULTRASONIC_CM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DC_MM_MODE, 10);
        case EV3_ULTRASONIC_MM:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DC_MM_MODE,  1);
        case EV3_ULTRASONIC_IN:
            return distanceRead(port, EV3_ULTRASONIC_SENSOR_DC_IN_MODE,  1);
        default:
            return INT_MIN;
    }
}

int distanceRead(int port, int mode, int divisor) {
    setEV3UltrasonicSensorMode(port, mode);

    DATA16 data = 0;
    int retval = readFromUART(port, (DATA8 *) &data, sizeof(data));
    if (retval < 0)
        return INT_MIN;

    return data / divisor;
}

int ReadEV3UltrasonicSensorListen(int port)  {
    setEV3UltrasonicSensorMode(port, EV3_ULTRASONIC_SENSOR_LISTEN_MODE);

    DATA8 data = 0;
    int retval = readFromUART(port, &data, sizeof(data));

    if (retval < 0)
        return INT_MIN;

    return data != 0;
}

int FireEV3UltrasonicSensor(int port) {
    uint8_t cmd = EV3_ULTRASONIC_FIRE_COMMAND;

    int retval = writeToUART(port, (DATA8*) &cmd, sizeof(cmd));
    if (retval != sizeof(cmd))
        return INT_MIN;

    return 0;
}

void setEV3UltrasonicSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Ultrasonic, port, EV3_ULTRASONIC_SENSOR_TYPE, mode);
}

void exitEV3UltrasonicSensor (int port) {

}

