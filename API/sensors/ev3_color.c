#include "inputs/ev3_input_uart.h"
#include "ev3_command.h"
#include "ev3_color.h"
#include "utility.h"

#define EV3_COLOR_SENSOR_TYPE           29
#define EV3_COLOR_SENSOR_DEFAULT_MODE   EV3_COLOR_SENSOR_REFLECT_MODE

SensorHandler * EV3Color = &(SensorHandler){
        .Init = initEV3ColorSensor,
        .Exit = exitEV3ColorSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initEV3ColorSensor(int port) {
    setEV3ColorSensorMode(port, EV3_COLOR_SENSOR_DEFAULT_MODE);
    return true;
}

int ReadEV3ColorSensorLight(int port, EV3ColorLightReadingMode lightMode) {
    int sensorMode = getEV3ColorSensorModeFromReadingLightMode(lightMode);
    return readEV3ColorSensorRawValue(port, sensorMode);
}

int getEV3ColorSensorModeFromReadingLightMode(EV3ColorLightReadingMode readingLightMode) {
    if (readingLightMode == ReflectedLight) {
        return EV3_COLOR_SENSOR_REFLECT_MODE;
    } else {
        return EV3_COLOR_SENSOR_AMBIENT_MODE;
    }
}

Color ReadEV3ColorSensor(int port) {
    return readEV3ColorSensorRawValue(port, EV3_COLOR_SENSOR_COLOR_MODE);
}

int readEV3ColorSensorRawValue(int port, int mode) {
    setEV3ColorSensorMode(port, mode);

    DATA8 data;
    int readResult = readFromUART(port, &data, 1);
    if (readResult < 0) {
        return -1;
    }
    return data;
}

void setEV3ColorSensorMode(int port, int mode) {
    setUARTSensorHandlerMode(EV3Color, port, EV3_COLOR_SENSOR_TYPE, mode);
}


int ReadEV3ColorSensorRGB(int port, RGB* rgb) {
    setEV3ColorSensorMode(port, EV3_COLOR_SENSOR_RGB_MODE);
    EV3Color->currentSensorMode[port] = EV3_COLOR_SENSOR_RGB_MODE;

    /**
    * The first 6 bytes in data are the colors: 2 byte for each color.
    * The range of each color value is from 0 to 1023.
    */
    uint8_t data[6];
    int res = readFromUART(port, (DATA8*) data, sizeof(data) / sizeof(uint8_t));
    if (res < 0) {
        return -1;
    }
    rgb->red    = data[0] + (data[1] << 8u);
    rgb->green  = data[2] + (data[3] << 8u);
    rgb->blue   = data[4] + (data[5] << 8u);
    return 0;
}

void exitEV3ColorSensor(int port) {

}
