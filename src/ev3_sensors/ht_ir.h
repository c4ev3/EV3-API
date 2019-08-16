#ifndef EV3_API_HT_IR_H
#define EV3_API_HT_IR_H

#include "ev3_sensors.h"

#define HT_IR_SENSOR_DEFAULT_MODE 0

// TODO: Specify version 2
extern SensorHandler * HTIr;

typedef enum HTIrReadingMode {
    Modulated,
    Unmodulated
} HTIrReadingMode;

bool initHTIrSensor(int port);

int ReadHTIrSensor(int port, HTIrReadingMode mode);

void exitHTIrSensor(int port);

/* Utility functions */
int getHTIrRegisterForReadingMode(HTIrReadingMode mode);

#endif //EV3_API_HT_IR_H
