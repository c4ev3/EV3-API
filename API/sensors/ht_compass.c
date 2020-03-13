#include "inputs/ev3_input_iic.h"
#include "ev3_sensors.h"
#include "ev3_constants.h"
#include "ev3_sensors/ht_compass.h"

//
// PRIVATE DECLARATIONS
//

static bool initHTCompassSensor(int port);

static void exitHTCompassSensor(int port);

//
// GLOBAL DATA
//

SensorHandler * HTCompass = &(SensorHandler){
    .Init = initHTCompassSensor,
    .Exit = exitHTCompassSensor,
    .currentSensorMode = {NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE}
};

static bool htCompassIsCalibrating[NUM_INPUTS] = {false, false, false, false};

//
// IMPLEMENTATION
//

bool initHTCompassSensor(int port) {
    initIICPort(port, HT_COMPASS_SENSOR_IIC_ADDRESS);
    HTCompass->currentSensorMode[port] = HT_COMPASS_SENSOR_DEFAULT_MODE;
    return true;
}


int ReadHTCompassSensor(int port, HTCompassReadingMode mode) {
    HTCompass->currentSensorMode[port] = mode;
    DATA8 data;
    readFromIIC(port, 0x42, &data, 1);

    if (mode == HTCompassCompass) {
        return ((data & 0xFF) << 1);
    } else {
        int temp = ((data & 0xFF) << 1);
        return  temp < 180 ? (-temp) : (360 - temp);
    }
}

void StartHTCompassCalibration(int port) {
    DATA8 request[] = {0x41, 0x43};
    writeToIIC(port, request, 2, 1, 0, 1);
    htCompassIsCalibrating[port] = true;
}

void StopHTCompassCalibration(int port) {
    DATA8 request[] = {0x41, 0x00};
    writeToIIC(port, request, 2, 1, 0, 1);
    htCompassIsCalibrating[port] = false;
}


void exitHTCompassSensor(int port) {
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        if (htCompassIsCalibrating[i]) {
            StopHTCompassCalibration(port);
        }
    }
}
