#include "../ev3_sensor.h"
#include "ev3_ultrasonic.h"


int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode) {
    SwitchSensorToModeIfNeeded(port, mode);

    DATA16 data;
    readFromUART(port, &data, 2);

    int distance = data & 0x0FFF;
    if (mode == CM) {
        distance /= 10;
    }
    return distance;
}

bool ReadEV3UltrasonicSensorListen(int port)  {
    SwitchSensorToModeIfNeeded(port, US_LISTEN);

    DATA8 data;
    readFromUART(port, &data, 1);

    return data & 0x0F;
}
