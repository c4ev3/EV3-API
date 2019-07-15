#include "nxt_sound.h"

int ReadNXTSoundSensor(int port, NXTSoundSensorMode mode) {
    SwitchSensorToModeIfNeeded(port, mode);
    DATA16 data = readOldDumbSensor(port);
    int temp = data;
    return (int)((1.0 - (temp/4095.0)) * 100.0); // ADC_RES = 4095
}
