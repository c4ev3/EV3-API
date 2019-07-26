#include "../../copied/lms2012/ev3_basictypes.h"
#include "../ev3_inputs/ev3_input_analog.h"
#include "../ev3_sensor.h"
#include "ev3_touch.h"

SensorHandler * EV3Touch = &(SensorHandler){
        .Init = initEV3Touch,
        .Exit = exitEV3Touch
};


bool initEV3Touch(int port) {
    // analog sensor doesn't need any initialization
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