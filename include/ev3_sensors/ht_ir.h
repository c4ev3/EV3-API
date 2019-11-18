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

int ReadHTIrSensor(int port, HTIrReadingMode mode);

#endif //EV3_API_HT_IR_H
