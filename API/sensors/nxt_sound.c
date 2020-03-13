#include "inputs/ev3_input_analog.h"
#include "ev3_sensors.h"
#include "ev3_sensors/nxt_sound.h"

//
// PRIVATE DECLARATIONS
//

static bool initNXTSoundSensor (int port);

//
// GLOBAL DATA
//

SensorHandler * NXTSound = &(SensorHandler){
        .Init = initNXTSoundSensor,
        .Exit = NULL,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initNXTSoundSensor (int port) {
    NXTSound->currentSensorMode[port] = NXT_SOUND_SENSOR_DEFAULT_MODE;
    return true;
}

int ReadNXTSoundSensor(int port, NXTSoundReadingMode mode) {
    // TODO: How to switch reading mode for OldDumb sensor?
    DATA16 data = readOldDumbSensor(port);
    int temp = data;
    return (int)((1.0 - (temp/4095.0)) * 100.0); // ADC_RES = 4095
}
