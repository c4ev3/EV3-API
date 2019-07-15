#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "ev3_input_uart.h"
#include "../../../copied/lms2012/ev3_uart.h"

static bool initialized = false;
static int uartFile = 0;
static UART * uartSensors = NULL;

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
    initialized = true;
    return true;
}

int readFromUART(int sensorPort, DATA8 * buffer, int length) {
    if (!initialized) {
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
    if (!initialized) {
        return;
    }
    munmap(uartSensors, sizeof(UART));
    uartSensors = NULL;
    close(uartFile);
    uartFile = 0;
    initialized = false;
}
