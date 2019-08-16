#ifndef EV3_API_HT_COLOR_H
#define EV3_API_HT_COLOR_H

#include "ev3_sensors.h"

#define HT_COLOR_SENSOR_DEFAULT_MODE        0
#define HT_COLOR_SENSOR_PASSIVE_MODE        1
#define HT_COLOR_SENSOR_RAW_MODE            3

extern SensorHandler * HTColor;

typedef enum HTColorPowerMainsFrequency {
    PowerMains50Hz,
    PowerMains60Hz,
} HTColorPowerMainsFrequency;

typedef enum HTColorReadingMode {
    HTColorSensorDefaultMode,
    HTColorSensorPassiveMode,
    HTColorSensorRawMode
} HTColorReadingMode;

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
RGBA ReadHTColorSensorRGBA(int port, HTColorReadingMode mode);

void SetHTColorPowerMainsFrequency (int port, HTColorPowerMainsFrequency frequency);

void exitHTColorSensor();

/* utility functions */
int getHTColorSensorModeFromReadingMode(HTColorReadingMode readingMode);
int getHTColorSensorModeForMainsFrequency (HTColorPowerMainsFrequency frequency);
void setHTColorSensorMode(int port, int sensorMode);
void writeToHTColorModeRegister (int port, int value);

#endif //EV3_API_HT_COLOR_H
