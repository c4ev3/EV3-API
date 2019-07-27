#include "../ev3_inputs/ev3_input_uart.h"
#include "../ev3_wait.h"
#include "ev3_color.h"

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

int ReadEV3ColorSensorLight(int port, LightMode lightMode) {
    return readEV3ColorSensorRawValue(port, lightMode);
}

Color ReadEV3ColorSensor(int port) {
    return readEV3ColorSensorRawValue(port, EV3_COLOR_SENSOR_COLOR_MODE);
}

int readEV3ColorSensorRawValue(int port, int mode) {
    setEV3ColorSensorMode(port, mode);

    DATA8 data;
    int readResult = readFromUART(port, &data, 1);
    if (readResult < 0) { // TODO: Handle error
        return -1;
    }
    return data;
}

void setEV3ColorSensorMode(int port, int mode) {
    setUARTSensorMode(port, EV3_COLOR_SENSOR_TYPE, mode);
    EV3Color->currentSensorMode[port] = mode;
}


RGB ReadEV3ColorSensorRGB(int port) {
    setUARTSensorMode(port, EV3_COLOR_SENSOR_TYPE, EV3_COLOR_SENSOR_RGB_MODE);
    EV3Color->currentSensorMode[port] = EV3_COLOR_SENSOR_RGB_MODE;

    /**
	* The first 6 bytes in data are the colors: 2 byte for each color.
	* The range of each color value is from 0 to 1023.
	*/
    DATA8 data[6];
    readFromUART(port, data, 6);
    // TODO: Handle error
    return (RGB) {
            .red    = ((uint8_t)data[0]) + (((uint8_t) data[1]) << 8u),
            .green  = ((uint8_t)data[2]) + (((uint8_t) data[3]) << 8u),
            .blue   = ((uint8_t)data[4]) + (((uint8_t) data[5]) << 8u)
    };
}

void exitEV3ColorSensor(int port) {

}