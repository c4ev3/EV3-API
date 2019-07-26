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
    return true;
}

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



// copied from pxt: pxt-ev3/libs/core/input.ts
// TODO: clean and move declarations in .h
int getUartStatus(int port) {
    return uartSensors->Status[port];
}

int waitNonZeroUARTStatus(int port) {
    while (true) {
        int status = getUartStatus(port);
        if (status != 0) {
            return status;
        }
        Wait(25);
    }
}

void uartClearChange (int port) {
    while (1) {
        int status = getUartStatus(port);

        if ((status & UART_DATA_READY) != 0 && (status & UART_PORT_CHANGED) == 0) {
            break;
        }

        devCon.Connection[port] = CONN_INPUT_UART;
        devCon.Type[port] = 0;
        devCon.Mode[port] = 0;

        ioctl(uartFile, UART_CLEAR_CHANGED, &devCon);

        uartSensors->Status[port] = getUartStatus(port) & 0xfffe;

        Wait(10);
    }
}


bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode) {
    while (1) {

        devCon.Connection[port] = CONN_INPUT_UART;
        devCon.Type[port] = sensorType;
        devCon.Mode[port] = sensorMode;

        // TODO: Handle with care
        int res = ioctl(uartFile, UART_SET_CONN, &devCon);


        int status = waitNonZeroUARTStatus(port);

        if (status & UART_PORT_CHANGED) {
            uartClearChange(port);
        } else {
            break;
        }
        Wait(10);

        //LcdTextf(1, 50, 10, "res %d", res);
        return true;
    }

}

// end of copied from pxt: pxt-ev3/libs/core/input.ts

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
