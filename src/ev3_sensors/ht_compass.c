#include "../../copied/lms2012/ev3_uart.h"
#include "../ev3_inputs/ev3_input_iic.h"
#include "../ev3_constants.h"
#include "../ev3_sensor.h"
#include "ht_compass.h"


#define HT_COMPASS_SENSOR_IIC_ADDRESS 0x01

SensorHandler * HTCompass = &(SensorHandler){
    .Init = initHTCompassSensor,
    .Exit = exitHTCompassSensor
};

static bool htCompassIsCalibrating[NUM_INPUTS] = {false, false, false, false};


bool initHTCompassSensor(int port) {
    initIICPort(port);
}


int ReadHTCompassSensor(int port, HTCompassMode mode) {
    DATA8 data;
    readFromIIC(port, &data, 1);

    if (mode == HTCompassCompass) {
        return ((data & 0xFF) << 1);
    } else {
        int temp = ((data & 0xFF) << 1);
        return  temp < 180 ? (-temp) : (360 - temp);
    }
}

void StartHTCompassCalibration(int port) {
    DATA8 request[] = {0x41, 0x43};
    writeIicRequestUsingIoctl(port, HT_COMPASS_SENSOR_IIC_ADDRESS, request, 2, 1, 0, 1);
    htCompassIsCalibrating[port] = true;
}

void StopHTCompassCalibration(int port) {
    DATA8 request[] = {0x41, 0x00};
    writeIicRequestUsingIoctl(port, HT_COMPASS_SENSOR_IIC_ADDRESS, request, 2, 0, 100, 1);
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
