#ifndef HT_COLOR_PRIVATE_H
#define HT_COLOR_PRIVATE_H

#include "ht_color.h"

bool initHTColorSensorV2(int port);

int getHTColorSensorV2ModeFromReadingMode(HTColorV2ReadingMode readingMode);

int getHTColorSensorV2ModeForMainsFrequency(HTColorV2PowerMainsFrequency frequency);

void setHTColorSensorV2Mode(int port, int sensorMode);

void writeToHTColorV2ModeRegister(int port, int value);

int readHTColorV2RGBA1ByteColor(int port, RGBA* result);

int readHTColorV2RGBA2ByteColor(int port, RGBA* result);

#endif
