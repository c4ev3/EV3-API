#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../../copied/lms2012/ev3_iic.h"
#include "ev3_input_iic.h"
#include "ev3_input.h"

#define IIC_SENSOR_TYPE 100
#define IIC_SENSOR_BYTE_MODE 0

static bool ev3IICInputInitialized = false;
static int iicFile = 0;
static IIC* iicSensors = 0;

extern DEVCON devCon;

bool initEV3IICnput() {
    if (ev3IICInputInitialized) {
        return false;
    }
    iicFile = open("/dev/lms_iic", O_RDWR | O_SYNC);
    if (iicFile == -1) {
        return false;
    }
    iicSensors = (IIC*)mmap(0, sizeof(IIC), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, iicFile, 0);
    if (iicSensors == MAP_FAILED) {
        close(iicFile);
        return false;
    }
    ev3IICInputInitialized = true;
    return true;
}


bool initIICPort(int port) {
    devCon.Connection[port] = CONN_NXT_IIC;
    devCon.Type[port] = IIC_SENSOR_TYPE;
    devCon.Mode[port] = IIC_SENSOR_BYTE_MODE;
    ioctl(iicFile, IIC_SET_CONN, &devCon);
    return true;
}

int readFromIIC (int sensorPort, DATA8 * buffer, int length) {
    if (!ev3IICInputInitialized) {
        return -1;
    }
    uint16_t slot = iicSensors->Actual[sensorPort];
    DATA8 * data = iicSensors->Raw[sensorPort][slot];
    int toRead = length > IIC_DATA_LENGTH ? IIC_DATA_LENGTH : length;
    memcpy(buffer, data, toRead);
    return toRead;
}

/**
 * Writes to the IIC device connected to the sensor port.
 * If repeatTimes is zero, the message will be sent for ever until a new call this function is made.
 * When reèeatTimes is 1, the message will be sent only one time.
*/
void writeIicRequestUsingIoctl(int sensorPort, int address, DATA8 toWrite[], int toWriteLength, int repeatTimes, int repeatInterval,  int responseLength) {
    static IICDAT IicDat;
    IicDat.Port = sensorPort;
    IicDat.Time = repeatInterval;
    IicDat.Repeat = repeatTimes;
    IicDat.RdLng = -responseLength;

    // the first byte of data is the length of data to send
    IicDat.WrLng = toWriteLength + 1;
    IicDat.WrData[0] = address;

    int i;
    for (i = 0; i < toWriteLength; i++) {
        IicDat.WrData[i + 1] = toWrite[i];
    }

    IicDat.Result = -1;
    while (IicDat.Result) {
        ioctl(iicFile, IIC_SETUP, &IicDat);
    }
}

void exitEV3IICInput() {
    if (!ev3IICInputInitialized) {
        return;
    }
    munmap(iicSensors, sizeof(IIC));
    iicSensors = NULL;
    close(iicFile);
    iicFile = 0;
    ev3IICInputInitialized = false;
}
