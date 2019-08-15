#ifndef EV3_API_HT_IR_H
#define EV3_API_HT_IR_H

#include "ev3_sensors.h"

extern SensorHandler * HTIr;

typedef enum HTIrSensorMode {
    Modulated,
    Unmodulated
} HTIrSensorMode;

bool initHTIrSensor(int port);

int ReadHTIrSensor(int port, HTIrSensorMode mode);

void exitHTIrSensor(int port);

/* Utility functions */
int getHTIrRegisterForMode (HTIrSensorMode mode);

#endif //EV3_API_HT_IR_H
