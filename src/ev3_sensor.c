#include "ev3_sensor.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "ev3.h"
#include "../copied/lms2012/ev3_analog.h"
#include "../copied/lms2012/ev3_iic.h"
#include "../copied/lms2012/ev3_uart.h"

/***********************************/
/**
 * The EV3 firmware uses a build-in device type list in order to know how to read
 * data from the various external devices.
 * The list of types can be found in the EV3 firmware source code or in the "EV3
 * Firmware Development Kit" document (downloadable from the Lego website).
 */

// IIC 
#define IIC_TYPE 100
#define IIC_BYTE_MODE 0

// NXT Compass
#define NXT_COMPASS_IIC_ADDRESS 0x01

// NXT IR
#define NXT_IR_IIC_ADDRESS 0x08
#define NXT_IR_REGISTER_DC 0x42
#define NXT_IR_REGISTER_AC 0x49

// NXT Temperture
#define NXT_TEMP_TYPE 6
#define NXT_TEMP_C_MODE 0	// Temperature in C
#define NXT_TEMP_F_MODE 1	// Temperature in F


// Mode of ev3_inputs
int sensor_setup_NAME[INPUTS];
int ir_sensor_channel[INPUTS];




enum      IIC_STATE
{
  IIC_IDLE,
  IIC_INIT,
  IIC_RESTART,
  IIC_ENABLE,
  IIC_NXT_TEMP_START,
  IIC_NXT_TEMP_WRITE,
  IIC_NXT_TEMP_READ,
  IIC_MANUFACTURER_START,
  IIC_MANUFACTURER_WRITE,
  IIC_MANUFACTURER_READ,
  IIC_TYPE_START,
  IIC_TYPE_WRITE,
  IIC_TYPE_READ,
  IIC_SETUP_WAIT,
  IIC_SETUP_START,
  IIC_SETUP_WRITE,
  IIC_SETUP_READ,
  IIC_WAITING,
  IIC_WRITING,
  IIC_READING,
  IIC_REPEAT,
  IIC_ERROR,
  IIC_EXIT,
  IIC_STATES
};

DEVCON devCon;

int setSensorMode(int sensorPort, int name) {
    sensor_setup_NAME[sensorPort] = name;
    char pins;
    switch (name) {
            // NXT
        case NXT_IR_SEEKER_DC:
        case NXT_IR_SEEKER_AC:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = IIC_TYPE;
            devCon.Mode[sensorPort] = IIC_BYTE_MODE;
            break;
        case NXT_TEMP_C:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = NXT_TEMP_TYPE;
            devCon.Mode[sensorPort] = NXT_TEMP_C_MODE;
            break;
        case NXT_TEMP_F:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = NXT_TEMP_TYPE;
            devCon.Mode[sensorPort] = NXT_TEMP_F_MODE;
            break;
        case NXT_COMPASS_COMPASS:
        case NXT_COMPASS_ANGLE:
            devCon.Connection[sensorPort] = CONN_NXT_IIC;
            devCon.Type[sensorPort] = IIC_TYPE;
            devCon.Mode[sensorPort] = IIC_BYTE_MODE;
            break;
        default:
            return -1;
    }
    return 0;
}

