#include "../ev3_wait.h"
#include "ev3_color.h"


int ReadEV3ColorSensorLight(int sensorPort, LightMode mode) {
    SwitchSensorToModeIfNeeded(sensorPort, mode);


    // TODO: Modify mode
    DATA8 data;
    int readResult = readFromUART(sensorPort, &data, 1);
    if (readResult < 0) { // TODO: Handle error
        return -1;
    }
    return data;
}

Color ReadEV3ColorSensor(int sensorPort) {
    SwitchSensorToModeIfNeeded(sensorPort, COL_COLOR);


    // TODO: Modify mode
    DATA8 data;
    int readResult = readFromUART(sensorPort, &data, 1);
    if (readResult < 0) { // TODO: Handle error
        return -1;
    }
    return data;
}


RGB ReadEV3ColorSensorRGB(int sensorPort) {
    SwitchSensorToModeIfNeeded(sensorPort, COL_RGB);


    /**
	* The first 6 bytes in data are the colors: 2 byte for each color.
	* The range of each color value is from 0 to 1023.
	*/
    DATA8 data[6];
    readFromUART(sensorPort, data, 6);
    // TODO: Handle error

    return (RGB) {
            .red    = ((uint8_t)data[0]) + (((uint8_t) data[1]) << 8u),
            .green  = ((uint8_t)data[2]) + (((uint8_t) data[3]) << 8u),
            .blue   = ((uint8_t)data[4]) + (((uint8_t) data[5]) << 8u)
    };
}

