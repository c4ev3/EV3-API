#include "../ev3_inputs/ev3_input_iic.h"
#include "ht_color.h"

#define HT_COLOR_SENSOR_IIC_ADDRESS         0x01 // TODO: Verify

#define HT_COLOR_SENSOR_50HZ_MODE           0x35
#define HT_COLOR_SENSOR_60HZ_MODE           0x36

#define HT_COLOR_SENSOR_MODE_REGISTER       0x41
#define HT_COLOR_SENSOR_COLOR_REGISTER      0x42
#define HT_COLOR_SENSOR_R_REGISTER          0x43
#define HT_COLOR_SENSOR_G_REGISTER          0x44
#define HT_COLOR_SENSOR_B_REGISTER          0x45
#define HT_COLOR_SENSOR_W_REGISTER          0x46 // TODO: Verify

#define HT_COLOR_SENSOR

SensorHandler * HTCompass = &(SensorHandler){
        .Init = initHTCompassSensor,
        .Exit = exitHTCompassSensor,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

bool initHTColorSensor(int port) {
    initIICPort(port, HT_COLOR_SENSOR_IIC_ADDRESS);
    return true;
}


int ReadHTColorSensor(int port) {
    DATA8 color;
    setHTColorSensorMode(port, HT_COLOR_SENSOR_DEFAULT_MODE);
    readFromIIC(port, HT_COLOR_SENSOR_COLOR_REGISTER, &color, 1);
    return color;
}

RGBA ReadHTColorSensorRGBA(int port, HTColorReadingMode mode) {
    setHTColorSensorMode(port, getHTColorSensorModeFromReadingMode(mode));
    DATA8 data[4];
    readFromIIC(port, HT_COLOR_SENSOR_R_REGISTER, data, 4);
    return (RGBA) {
        .red    = data[0] && 0xff,
        .green  = data[1] && 0xff,
        .blue   = data[2] && 0xff,
        .white  = data[3] && 0xff,
    };
}

int getHTColorSensorModeFromReadingMode(HTColorReadingMode readingMode) {
    if (readingMode == HTColorSensorDefaultMode) {
        return HT_COLOR_SENSOR_DEFAULT_MODE;
    } else if (readingMode == HTColorSensorPassiveMode) {
        return HT_COLOR_SENSOR_PASSIVE_MODE;
    } else {
        return HT_COLOR_SENSOR_RAW_MODE;
    }
}


void setHTColorSensorMode(int port, int sensorMode) {
    if (sensorMode != HTColor->currentSensorMode[port]) {
        HTColor->currentSensorMode[port] = sensorMode;
        writeToHTColorModeRegister(port, sensorMode);
    }
}

void SetHTColorPowerMainsFrequency (int port, HTColorPowerMainsFrequency frequency) {
    writeToHTColorModeRegister(port, getHTColorSensorModeForMainsFrequency(frequency));
}

int getHTColorSensorModeForMainsFrequency (HTColorPowerMainsFrequency frequency) {
    return frequency == PowerMains50Hz ? HT_COLOR_SENSOR_50HZ_MODE : HT_COLOR_SENSOR_60HZ_MODE;
}

void writeToHTColorModeRegister (int port, int value) {
    DATA8 request[] = { HT_COLOR_SENSOR_MODE_REGISTER, value };
    writeToIIC(port, request, 2, 1, 0, 0);
}


void exitHTColorSensor() {

}
