#include "inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "ev3_ultrasonic.h"
#include "utility.h"

#define EV3_ULTRASONIC_SENSOR_TYPE              30

SensorHandler * EV3Ultrasonic = &(SensorHandler){
        .Init = initEV3UltrasonicSensor,
        .Exit = exitEV3UltrasonicSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initEV3UltrasonicSensor (int port) {
    setEV3UltrasonicSensorMode(port, EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE);
    return true;
}


int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode) {
    setEV3UltrasonicSensorMode(port, getEV3UltrasonicSensorModeFromDistanceUnit(mode));

    DATA16 data;
    if (readFromUART(port, (DATA8 *) &data, 2) == -1) {
        return -1;
    }

    int distance = data & 0x0FFF;
    if (mode == EV3_ULTRASONIC_CM) {
        distance /= 10;
    }
    return distance;
}

int getEV3UltrasonicSensorModeFromDistanceUnit(EV3UltrasonicDistanceUnit mode){
    if (mode == EV3_ULTRASONIC_IN) {
        return EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE;
    } else {
        return EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE;
    }
}

bool ReadEV3UltrasonicSensorListen(int port)  {
    setEV3UltrasonicSensorMode(port, EV3_ULTRASONIC_SENSOR_LISTEN_MODE);

    DATA8 data;
    readFromUART(port, &data, 1);

    return data & 0x0F;
}

void setEV3UltrasonicSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Ultrasonic, port, EV3_ULTRASONIC_SENSOR_TYPE, mode);
}

void exitEV3UltrasonicSensor (int port) {

}

