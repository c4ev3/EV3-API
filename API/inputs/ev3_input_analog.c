#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <contrib/lms2012/ev3_analog.h>
#include "ev3_input_analog.h"


static int analogFile = 0;
static ANALOG * analogSensors = NULL;
static bool ev3AnalogInputInitialized = false;

ANALOG * initEV3AnalogInput(){
    if (ev3AnalogInputInitialized) {
        return NULL;
    }
    analogFile = open("/dev/lms_analog", O_RDWR | O_SYNC);
    if (analogFile == -1) {
        return NULL;
    }
    analogSensors = (ANALOG*) mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, analogFile, 0);
    if (analogSensors == MAP_FAILED) {
        close(analogFile);
        return NULL;
    }
    ev3AnalogInputInitialized = true;
    return analogSensors;
}

DATA16 readNewDumbSensor(int sensorPort) {
    return analogSensors->InPin6[sensorPort];
}

DATA16 readOldDumbSensor(int sensorPort) {
    return analogSensors->InPin1[sensorPort];
}

void exitEV3AnalogInput(){
    if (!ev3AnalogInputInitialized) {
        return;
    }
    munmap(analogSensors, sizeof(ANALOG));
    analogSensors = NULL;
    close(analogFile);
    analogFile = 0;
    ev3AnalogInputInitialized = false;
}
