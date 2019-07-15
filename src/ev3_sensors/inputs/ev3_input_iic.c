#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "ev3_input_iic.h"
#include "../../../copied/lms2012/ev3_iic.h"

static bool initialized = false;
static int iicFile = 0;
static IIC* iicSensors = 0;

// TODO: Remove
int g_iicFile;

bool initEV3IICnput() {
    if (initialized) {
        return false;
    }
    g_iicFile = iicFile = open("/dev/lms_iic", O_RDWR | O_SYNC);
    if (iicFile == -1) {
        return false;
    }
    iicSensors = (IIC*)mmap(0, sizeof(IIC), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, g_iicFile, 0);
    if (iicSensors == MAP_FAILED) {
        close(iicFile);
        return false;
    }
    initialized = true;
    return true;
}

int readFromIIC (int sensorPort, DATA8 * buffer, int length) {
    if (!initialized) {
        return -1;
    }
    uint16_t slot = iicSensors->Actual[sensorPort];
    DATA8 * data = iicSensors->Raw[sensorPort][slot];
    int toRead = length > IIC_DATA_LENGTH ? IIC_DATA_LENGTH : length;
    memcpy(buffer, data, toRead);
    return toRead;
}

void exitEV3IICInput() {
    if (!initialized) {
        return;
    }
    munmap(iicSensors, sizeof(IIC));
    iicSensors = NULL;
    close(iicFile);
    iicFile = 0;
    initialized = false;
}
