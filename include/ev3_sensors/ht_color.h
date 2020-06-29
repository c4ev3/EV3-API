#ifndef HT_COLOR_H
#define HT_COLOR_H

#include "ev3_sensors.h"

#define HT_COLOR_SENSOR_V2_IIC_ADDRESS         0x01

#define HT_COLOR_SENSOR_V2_50HZ_MODE           0x35
#define HT_COLOR_SENSOR_V2_60HZ_MODE           0x36

#define HT_COLOR_SENSOR_V2_MODE_REGISTER       0x41
#define HT_COLOR_SENSOR_V2_COLOR_REGISTER      0x42
#define HT_COLOR_SENSOR_V2_R_1BYTE_REGISTER    0x43
#define HT_COLOR_SENSOR_V2_G_1BYTE_REGISTER    0x44
#define HT_COLOR_SENSOR_V2_B_1BYTE_REGISTER    0x45
#define HT_COLOR_SENSOR_V2_W_1BYTE_REGISTER    0x46

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

int ReadHTColorSensorV2(int port);

/**
 * Read RGBA values. Each value ranges from 0 to 1023
 * @param port port to which the sensor is connected
 * @param mode HTColorV2ReadingMode
 * @param result struct to which the result will be written  (output variable)
 * @return -1 if there was an error
 */
int ReadHTColorSensorV2RGBA(int port, HTColorV2ReadingMode mode, RGBA* result);

/**
 * Prepare the sensor to read values in an environment where mains power uses the
 * specified frequency
 * @param port port to which the sensor is connected
 * @param frequency frequency
 */
void SetHTColorV2PowerMainsFrequency(int port, HTColorV2PowerMainsFrequency frequency);


#endif //EV3_API_HT_COLOR_H
