#include "../ev3_inputs/ev3_input_uart.h"
#include "../ev3_sensor.h"
#include "ev3_ultrasonic.h"

#define EV3_ULTRASONIC_SENSOR_TYPE              30
#define EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE  0
#define EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE  1
#define EV3_ULTRASONIC_SENSOR_LISTEN_MODE       2

int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode) {
    setUARTSensorModeIfNeeded(port, EV3_ULTRASONIC_SENSOR_TYPE, getEV3UltrasonicSensorDistanceModeConstant(mode));
    //setUARTSensorMode(port, EV3_ULTRASONIC_SENSOR_TYPE, getEV3UltrasonicSensorDistanceModeConstant(mode));


    DATA16 data;
    readFromUART(port, &data, 2);

    int distance = data & 0x0FFF;
    if (mode == CM) {
        distance /= 10;
    }
    return distance;
}

int getEV3UltrasonicSensorDistanceModeConstant(EV3UltrasonicDistanceUnit mode){
    if (mode == IN) {
        return EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE;
    } else {
        return EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE;
    }
}

bool ReadEV3UltrasonicSensorListen(int port)  {
    setUARTSensorModeIfNeeded(port, EV3_ULTRASONIC_SENSOR_TYPE, EV3_ULTRASONIC_SENSOR_LISTEN_MODE);

    DATA8 data;
    readFromUART(port, &data, 1);

    return data & 0x0F;
}
