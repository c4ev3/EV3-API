#ifndef EV3_API_HT_COLOR_H
#define EV3_API_HT_COLOR_H

#include "ev3_sensors.h"

#define HT_COLOR_SENSOR_V2_DEFAULT_MODE        0
#define HT_COLOR_SENSOR_V2_PASSIVE_MODE        1
#define HT_COLOR_SENSOR_V2_RAW_MODE            3

extern SensorHandler * HTColorV2;

typedef enum HTColorV2PowerMainsFrequency {
    PowerMains50Hz,
    PowerMains60Hz,
} HTColorV2PowerMainsFrequency;

typedef enum HTColorV2ReadingMode {
    HTColorSensorDefaultMode,
    HTColorSensorPassiveMode,
    HTColorSensorRawMode
} HTColorV2ReadingMode;

typedef struct RGBA {
    int red;
    int blue;
    int green;
    int white;
} RGBA;

bool initHTColorSensorV2(int port);

int ReadHTColorSensorV2(int port);

/**
 * Read RGBA values. Each value ranges from 0 to 1023
 * @param port
 * @return
 */
RGBA ReadHTColorSensorV2RGBA(int port, HTColorV2ReadingMode mode);

void SetHTColorV2PowerMainsFrequency(int port, HTColorV2PowerMainsFrequency frequency);

void exitHTColorSensorV2();

/* utility functions */
int getHTColorSensorV2ModeFromReadingMode(HTColorV2ReadingMode readingMode);
int getHTColorSensorV2ModeForMainsFrequency(HTColorV2PowerMainsFrequency frequency);
void setHTColorSensorV2Mode(int port, int sensorMode);
void writeToHTColorV2ModeRegister(int port, int value);
RGBA readHTColorV2RGBA1ByteColor(int port);
RGBA readHTColorV2RGBA2ByteColor(int port);

#endif //EV3_API_HT_COLOR_H
