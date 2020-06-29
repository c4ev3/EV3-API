#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include "contrib/lms2012/ev3_uart.h"
#include "ev3_command.h"
#include "ev3_constants.h"
#include "ev3_input_uart.h"

static bool ev3UARTInputInitialized;
static int uartFile;
static UART * uartSensors;

extern DEVCON devCon;

bool initEV3UARTInput(ANALOG * analogSensors) {
    uartFile = open("/dev/lms_uart", O_RDWR | O_SYNC);
    if (uartFile == -1) {
        return false;
    }
    uartSensors = (UART*)mmap(0, sizeof(UART), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, uartFile, 0);
    if (uartSensors == MAP_FAILED) {
        close(uartFile);
        return false;
    }
    ev3UARTInputInitialized = true;

    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        devCon.Connection[i] = analogSensors->InConn[i];
        devCon.Type[i] = TYPE_KEEP;
        devCon.Mode[i] = MODE_KEEP;
    }
    return true;
}


bool setUARTSensorMode(int port, DATA8 sensorType, DATA8 sensorMode) {
    /**
     * Procedure to set UART mode inspired by pxt: pxt-ev3/libs/core/input.ts
     */

    // update the persisted global state of connections
    devCon.Connection[port] = CONN_INPUT_UART;
    devCon.Type[port] = sensorType;
    devCon.Mode[port] = sensorMode;

    bool portChanged = true;
    // note: The while loop is necessary for the case when this UART was
    //       deinitialized. In that case, the kernel does not take
    //       account of the mode to set the sensor in; it just powers
    //       up the sensor and sets it to mode 0. Therefore, to also
    //       set the correct mode, a second syscall round has to be made.
    while (portChanged) {
        // note: This write here is necessary even though the kernel does
        //       the same operation to its shared memory. For some reason
        //       the change does not take effect immediately but only
        //       after some time. To prevent erroneous continuation,
        //       the flag is removed here as well.
        //     + a similar code can be found in the lms2012 VM.
        //     + adding volatile to uartSensors didn't make a difference
        //       (and lms2012 VM does not use it as well)
        //     + using msync() also didn't seem to make a difference
        uartSensors->Status[port] &= ~UART_DATA_READY;
        // send the request to the kernel
        ioctl(uartFile, UART_SET_CONN, &devCon);

        // wait until the sensor is up (in whatever mode)
        waitUartReady(port, &portChanged);
    }

    return true;
}

void waitUartReady(int port, bool *portChanged) {
    UARTCTL cmd = { .Port = port };

    *portChanged = false;
    while (true) {
        // read current port state
        int status = uartSensors->Status[port];

        // kernel has gone through sensor initialization sequance
        if ((status & UART_PORT_CHANGED) != 0) {

            // signal that the device needs to be configured again
            *portChanged = true;

            // clear the flag in the kernel
            ioctl(uartFile, UART_CLEAR_CHANGED, &cmd);
            // and clear it in userspace as well
            // note: the write here is necessary for the same reason
            //       as the write above for UART_DATA_READY
            uartSensors->Status[port] &= ~UART_PORT_CHANGED;

        } else if ((status & UART_DATA_READY) != 0) {
            // we're ready, the device has sent some data
            break;
        }

        // wait until the device is ready
        Wait(10);
    }
}

void disableUART(int port) {
    // update the persisted global state of connections
    devCon.Connection[port] = CONN_NONE;
    devCon.Type[port] = TYPE_NONE;
    devCon.Mode[port] = MODE_KEEP;

    // bye bye
    ioctl(uartFile, UART_SET_CONN, &devCon);
}

int readFromUART(int sensorPort, DATA8 * buffer, int length) {
    if (!ev3UARTInputInitialized) {
        return -1;
    }
    uint16_t slot = uartSensors->Actual[sensorPort];
    DATA8 * data = uartSensors->Raw[sensorPort][slot];
    int toRead = length > UART_DATA_LENGTH
                        ? UART_DATA_LENGTH
                        : length;
    memcpy(buffer, data, toRead);
    return toRead;
}

bool writeToUART(int port, DATA8 * buffer, int length) {
    if (!ev3UARTInputInitialized) {
        return -1;
    }
    int msgLen = length > UART_DATA_LENGTH
                        ? UART_DATA_LENGTH
                        : length;
    int cmdLen = msgLen + 1;

    // copy message and prepend it with port number
    DATA8 cmd[UART_DATA_LENGTH + 1] = {0};
    cmd[0] = port;
    memcpy(cmd + 1, buffer, msgLen);

    // copied from lms2012 VM: probably to force confirmation of new data
    uartSensors->Status[port] &= ~UART_DATA_READY;
    // send the request to kernel
    int retval = write(uartFile, cmd, cmdLen);
    return retval == cmdLen;
}

void exitEV3UARTInput() {
    if (!ev3UARTInputInitialized) {
        return;
    }
    munmap(uartSensors, sizeof(UART));
    uartSensors = NULL;
    close(uartFile);
    uartFile = -1;
    ev3UARTInputInitialized = false;
}
