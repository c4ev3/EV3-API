#include "../../copied/lms2012/ev3_uart.h"
#include "../ev3_sensor.h"
#include "ht_compass.h"


#define NXT_COMPASS_IIC_ADDRESS 0x01

int ReadHTCompassSensor(int port, HTCompassMode mode) {
    SwitchSensorToModeIfNeeded(port, NXT_COMPASS_COMPASS);
    DATA8 data;
    readFromIIC(port, &data, 1);

    if (mode == HTCompassCompass) {
        return ((data & 0xFF) << 1);
    } else {
        int temp = ((data & 0xFF) << 1);
        return  temp < 180 ? (-temp) : (360 - temp);
    }
}

void StartHTCompassCalibration(int sensorPort) {
    DATA8 request[] = {0x41, 0x43};
    writeIicRequestUsingIoctl(sensorPort, NXT_COMPASS_IIC_ADDRESS, request, 2, 1, 0, 1);
}

void StopHTCompassCalibration(int sensorPort) {
    DATA8 request[] = {0x41, 0x00};
    writeIicRequestUsingIoctl(sensorPort, NXT_COMPASS_IIC_ADDRESS, request, 2, 0, 100, 1);
}
