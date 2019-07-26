#include "../ev3_inputs/ev3_input_analog.h"
#include "nxt_sound.h"

#define NXT_SOUND_SENSOR_TYPE       3
#define NXT_SOUND_SENSOR_DB_MODE    0
#define NXT_SOUND_SENSOR_DBA_MODE   1

SensorHandler * NXTSound = &(SensorHandler){
        .Init = initNXTSoundSensor,
        .Exit = exitNXTSoundSensor
};

bool initNXTSoundSensor (int port) {
    return true;
}

int ReadNXTSoundSensor(int port, NXTSoundSensorMode mode) {
    // TODO: Implement mode switching
    DATA16 data = readOldDumbSensor(port);
    int temp = data;
    return (int)((1.0 - (temp/4095.0)) * 100.0); // ADC_RES = 4095
}


void exitNXTSoundSensor (int port) {

}
