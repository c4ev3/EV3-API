#include "../../copied/lms2012/ev3_basictypes.h"
#include "../ev3_sensor.h"
#include "ev3_touch.h"

bool ReadEV3TouchSensor(int sensorPort) {
    DATA16 data = readNewDumbSensor(sensorPort);
    int temp = data / 256;
    return temp != 0;
}

