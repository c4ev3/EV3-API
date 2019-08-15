#ifndef EV3_API_HT_COLOR_H
#define EV3_API_HT_COLOR_H

#include "ev3_sensors.h"

extern SensorHandler * HTColor;

typedef enum HTColorPowerMainsFrequency {
    PowerMains50Hz,
    PowerMains60Hz,
} HTColorPowerMainsFrequency;

typedef struct RGBA {
    int red;
    int blue;
    int green;
    int white;
} RGBA;

bool initHTColorSensor(int port);

int ReadHTColorSensor(int port);

/**
 * Read RGBA values. Each value ranges from 0 to 255
 * @param port
 * @return
 */
RGBA ReadHTColorSensorRGBA(int port);

void SetHTColorPowerMainsFrequency (int port, HTColorPowerMainsFrequency frequency);

void exitHTColorSensor();

/* utility functions */
int getHTColorModeForMainsFrequency (HTColorPowerMainsFrequency frequency);

#endif //EV3_API_HT_COLOR_H
