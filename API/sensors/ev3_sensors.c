#include <stddef.h>
#include "ev3_constants.h"
#include "ev3_sensors.h"


static SensorHandler *currentSensorHandlers[NUM_INPUTS];

bool SensorsInit() {
    return true;
}

bool SetAllSensors(SensorHandler *port1, SensorHandler *port2, SensorHandler *port3, SensorHandler *port4) {
    SensorHandler * sensorHandlers[] = {port1, port2, port3, port4};
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        if (sensorHandlers[i] != NULL) {
            bool res = SetSensor(i, sensorHandlers[i]);
            if (!res) {
                exitSensorsUntilPort(sensorHandlers, i);
                return false;
            }
        }
    }
    return true;
}

bool SetSensor(int port, SensorHandler *sensor) {
    currentSensorHandlers[port] = sensor;
    return currentSensorHandlers[port]->Init(port);
}

void exitSensorsUntilPort(SensorHandler * sensorHandlers[], int lastPortExcluded) {
    for (int i = 0; i < lastPortExcluded; i++) {
        if (sensorHandlers[i] != NULL) {
            sensorHandlers[i]->Exit(i);
        }
    }
}

SensorHandler * GetSensor(int port) {
    return currentSensorHandlers[port];
}

void SensorsExit () {
    int i;
    for (i = 0; i < NUM_INPUTS; i++) {
        SensorHandler * sensor = currentSensorHandlers[i];
        if (sensor != NULL && sensor->Exit != NULL) {
            sensor->Exit(i);
            currentSensorHandlers[i] = NULL;
        }
    }
}
