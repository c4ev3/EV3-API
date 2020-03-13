#include "inputs/ev3_input_iic.h"
#include "ev3_sensors.h"
#include "ev3_sensors/ht_color.h"


//
// PRIVATE DECLARATIONS
//

static bool initHTColorSensorV2(int port);

static int getHTColorSensorV2ModeFromReadingMode(HTColorV2ReadingMode readingMode);

static int getHTColorSensorV2ModeForMainsFrequency(HTColorV2PowerMainsFrequency frequency);

static void setHTColorSensorV2Mode(int port, int sensorMode);

static void writeToHTColorV2ModeRegister(int port, int value);

static int readHTColorV2RGBA1ByteColor(int port, RGBA* result);

static int readHTColorV2RGBA2ByteColor(int port, RGBA* result);

//
// GLOBAL DATA
//

SensorHandler * HTColorV2 = &(SensorHandler){
        .Init = initHTColorSensorV2,
        .Exit = NULL,
        .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

//
// IMPLEMENTATION
//

bool initHTColorSensorV2(int port) {
    initIICPort(port, HT_COLOR_SENSOR_V2_IIC_ADDRESS);
    return true;
}


int ReadHTColorSensorV2(int port) {
    DATA8 color;
    setHTColorSensorV2Mode(port, HT_COLOR_SENSOR_V2_DEFAULT_MODE);
    readFromIIC(port, HT_COLOR_SENSOR_V2_COLOR_REGISTER, &color, 1);
    return color;
}

int ReadHTColorSensorV2RGBA(int port, HTColorV2ReadingMode mode, RGBA* result) {
    setHTColorSensorV2Mode(port, getHTColorSensorV2ModeFromReadingMode(mode));
    if (mode == HTColorSensorDefaultMode) {
        return readHTColorV2RGBA1ByteColor(port, result);
    } else {
        return readHTColorV2RGBA2ByteColor(port, result);
    }
}

int readHTColorV2RGBA1ByteColor(int port, RGBA* result) {
    DATA8 data[4];
    if (readFromIIC(port, HT_COLOR_SENSOR_V2_R_1BYTE_REGISTER, data, 4) < 0) {
        return -1;
    }
    result->red    = ((data[0] & 0xff) * 1023) / 255;
    result->green  = ((data[1] & 0xff) * 1023) / 255;
    result->blue   = ((data[2] & 0xff) * 1023) / 255;
    result->white  = ((data[3] & 0xff) * 1023) / 255;
    return 0;
}

int readHTColorV2RGBA2ByteColor(int port, RGBA* result) {
    DATA8 data[8];
    if (readFromIIC(port, 0x42, data, 8) < 0) {
        return -1;
    }
    result->red    = ((data[0] & 0xff) << 8) | (data[1] & 0xff);
    result->green  = ((data[2] & 0xff) << 8) | (data[3] & 0xff);
    result->blue   = ((data[4] & 0xff) << 8) | (data[5] & 0xff);
    result->white  = ((data[6] & 0xff) << 8) | (data[7] & 0xff);
    return 0;
}

int getHTColorSensorV2ModeFromReadingMode(HTColorV2ReadingMode readingMode) {
    if (readingMode == HTColorSensorDefaultMode) {
        return HT_COLOR_SENSOR_V2_DEFAULT_MODE;
    } else if (readingMode == HTColorSensorPassiveMode) {
        return HT_COLOR_SENSOR_V2_PASSIVE_MODE;
    } else {
        return HT_COLOR_SENSOR_V2_RAW_MODE;
    }
}

void setHTColorSensorV2Mode(int port, int sensorMode) {
    if (sensorMode != HTColorV2->currentSensorMode[port]) {
        HTColorV2->currentSensorMode[port] = sensorMode;
        writeToHTColorV2ModeRegister(port, sensorMode);
    }
}

void SetHTColorV2PowerMainsFrequency(int port, HTColorV2PowerMainsFrequency frequency) {
    writeToHTColorV2ModeRegister(port, getHTColorSensorV2ModeForMainsFrequency(frequency));
}

int getHTColorSensorV2ModeForMainsFrequency(HTColorV2PowerMainsFrequency frequency) {
    return frequency == PowerMains50Hz ? HT_COLOR_SENSOR_V2_50HZ_MODE : HT_COLOR_SENSOR_V2_60HZ_MODE;
}

void writeToHTColorV2ModeRegister(int port, int value) {
    DATA8 request[] = { HT_COLOR_SENSOR_V2_MODE_REGISTER, value };
    writeToIIC(port, request, 2, 1, 0, 0);
}

