#include "inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "ev3_sensors/ev3_sensors.h"
#include "utility.h"

//
// PRIVATE DECLARATIONS
//

static bool initEV3ColorSensor(int port);

static void exitEV3ColorSensor(int port);

static int readByte(int port, int mode);

static void setEV3ColorSensorMode(int port, int mode);

//
// GLOBAL DATA
//

SensorHandler * EV3Color = &(SensorHandler){
        .Init = initEV3ColorSensor,
        .Exit = exitEV3ColorSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initEV3ColorSensor(int port) {
    setEV3ColorSensorMode(port, EV3_COLOR_SENSOR_DEFAULT_MODE);
    return true;
}

int ReadEV3ColorSensorReflectedLight(int port) {
    return readByte(port, EV3_COLOR_SENSOR_REFLECT_MODE);
}

int ReadEV3ColorSensorAmbientLight(int port) {
    return readByte(port, EV3_COLOR_SENSOR_AMBIENT_MODE);
}

Color ReadEV3ColorSensorColor(int port) {
    return readByte(port, EV3_COLOR_SENSOR_COLOR_MODE);
}

int ReadEV3ColorSensorColorRGB(int port, RGB* rgb) {
    setEV3ColorSensorMode(port, EV3_COLOR_SENSOR_RAW_COLOR_MODE);

    uint8_t data[8] = {0};
    int retval = readFromUART(port, (DATA8*) data, sizeof(data));
    if (retval < 0) {
        return INT_MIN;
    }

    rgb->red        = data[0] + (data[1] << 8u);
    rgb->green      = data[2] + (data[3] << 8u);
    rgb->blue       = data[4] + (data[5] << 8u);
    rgb->background = data[6] + (data[7] << 8u);
    return 0;
}

int ReadEV3ColorSensorRawReflectedLight(int port, RawReflect *refraw) {
    setEV3ColorSensorMode(port, EV3_COLOR_SENSOR_RAW_REFLECT_MODE);

    uint8_t data[4] = {0};
    int retval = readFromUART(port, (DATA8*) data, sizeof(data));
    if (retval < 0) {
        return INT_MIN;
    }

    int led_on  = data[0] + (data[1] << 8u);
    int led_off = data[2] + (data[3] << 8u);

    refraw->reflection = led_off - led_on; // make this similar to COL-REFLECT and RGB-RAW
    refraw->background = led_off;          // pass this unchanged
    return 0;
}

int readByte(int port, int mode) {
    setEV3ColorSensorMode(port, mode);

    DATA8 data = 0;
    int retval = readFromUART(port, &data, sizeof(data));
    if (retval < 0) {
        return INT_MIN;
    }
    return data;
}

void setEV3ColorSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Color, port, EV3_COLOR_SENSOR_TYPE, mode);
}

void exitEV3ColorSensor(int port) {

}
