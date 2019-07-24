#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "../ev3_lcd.h"
#include "../ev3_time.h"
#include "ev3_input_uart.h"
#include "../../copied/lms2012/ev3_uart.h"

static bool ev3UARTInputInitialized = false;
static int uartFile = 0;
static UART * uartSensors = NULL;

//int dcmFile = 0;

extern DEVCON devCon;

// TODO: Remove
int g_uartFile;

bool initEV3UARTInput() {
    g_uartFile = uartFile = open("/dev/lms_uart", O_RDWR | O_SYNC);
    if (uartFile == -1) {
        return false;
    }
    uartSensors = (UART*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, g_uartFile, 0);
    if (uartSensors == MAP_FAILED) {
        close(uartFile);
        return false;
    }
    ev3UARTInputInitialized = true;

    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        devCon.Connection[i] = -1;
    }
/*
    dcmFile = open("/dev/lms_dcm", O_WRONLY | O_SYNC);
    if (dcmFile == -1) {
        LcdTextf(1, 10, 10, "lms_dcm failed");
    } else {
        LcdTextf(1, 10, 10, "lms_dcm opened");
    }*/


    return true;
}

//char buf[9];



bool setUARTSensorModeIfNeeded (int port, DATA8 sensorType, DATA8 sensorMode) {
    if(isUARTSensorModeDifferent(port, sensorType, sensorMode)) {
        setUARTSensorMode(port, sensorType, sensorMode);
    }
}

bool isUARTSensorModeDifferent(int port, DATA8 sensorType, DATA8 sensorMode) {
    return devCon.Connection[port] != CONN_INPUT_UART ||
        devCon.Type[port] != sensorType ||
        devCon.Mode[port] != sensorMode;
}


bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode) {
    /*buf[0] = '-';
    buf[1] = '-';
    buf[2] = '-';
    buf[3] = '-';
    buf[4] = 0;
    buf[port] = 0x2D;
    write(dcmFile, buf, 4);*/

    //LcdTextf(1, 10, 10, "Set %d", port);

    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = sensorType;
    devCon.Mode[port] = sensorMode;

    // TODO: Handle with care
    int res = ioctl(uartFile, UART_SET_CONN, &devCon);
    //LcdTextf(1, 50, 10, "res %d", res);
    return true;
}

int readFromUART(int sensorPort, DATA8 * buffer, int length) {
    if (!ev3UARTInputInitialized) {
        return -1;
    }
    uint16_t slot = uartSensors->Actual[sensorPort];
    DATA8 * data = uartSensors->Raw[sensorPort][slot];
    int toRead = length > 8 ? 8 : length;
    memcpy(buffer, data, toRead);
    return toRead;
}

/*
int wait_no_zero_status(int sensorPort) {
    int status;
    int i=0;

    for(i=0;i<50;i++){
        status = g_uartSensors->Status[sensorPort];
        if(status != 0){
            break;
        }
        sleep(1);
    }
    return status;
}*/


void exitEV3UARTInput() {
    if (!ev3UARTInputInitialized) {
        return;
    }
    munmap(uartSensors, sizeof(UART));
    uartSensors = NULL;
    close(uartFile);
    uartFile = 0;
    ev3UARTInputInitialized = false;
}
