#include "../../copied/lms2012/ev3_basictypes.h"
#include "../ev3_inputs/ev3_input_analog.h"
#include "ev3_touch.h"

SensorHandler * EV3Touch = &(SensorHandler){
        .Init = initEV3Touch,
        .Exit = exitEV3Touch,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};


bool initEV3Touch(int port) {
    EV3Touch->currentSensorMode[port] = EV3_TOUCH_SENSOR_TOUCH_MODE;
    return true;
}

bool ReadEV3TouchSensor(int sensorPort) {
    DATA16 data = readNewDumbSensor(sensorPort);
    int temp = data / 256;
    return temp != 0;
}

void exitEV3Touch(int port) {
    // analog sensor doesn't need any de-initialization
}
