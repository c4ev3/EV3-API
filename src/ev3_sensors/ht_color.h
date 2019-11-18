#ifndef HT_COLOR_PRIVATE_H
#define HT_COLOR_PRIVATE_H

#include "../../include/ev3_sensors/ht_color.h"

bool initHTColorSensorV2(int port);

void exitHTColorSensorV2();

int getHTColorSensorV2ModeFromReadingMode(HTColorV2ReadingMode readingMode);

int getHTColorSensorV2ModeForMainsFrequency(HTColorV2PowerMainsFrequency frequency);

void setHTColorSensorV2Mode(int port, int sensorMode);

void writeToHTColorV2ModeRegister(int port, int value);

RGBA readHTColorV2RGBA1ByteColor(int port);

RGBA readHTColorV2RGBA2ByteColor(int port);

#endif
