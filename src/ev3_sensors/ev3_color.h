#ifndef EV3_API_EV3_COLOR_H
#define EV3_API_EV3_COLOR_H

#include "ev3_sensors.h"

#define EV3_COLOR_SENSOR_REFLECT_MODE   0
#define EV3_COLOR_SENSOR_AMBIENT_MODE   1
#define EV3_COLOR_SENSOR_COLOR_MODE     2
#define EV3_COLOR_SENSOR_RGB_MODE       4

extern SensorHandler * EV3Color;

typedef enum Color {
    None,
    Black,
    Blue,
    Green,
    Yellow,
    Red,
    White,
    Brown
} Color;

typedef struct RGB {
    int red;
    int blue;
    int green;
} RGB;

typedef enum EV3ColorLightReadingMode {
    ReflectedLight,
    AmbientLight
} EV3ColorLightReadingMode;

bool initEV3ColorSensor(int port);

int ReadEV3ColorSensorLight(int port, EV3ColorLightReadingMode mode);

Color ReadEV3ColorSensor(int port);

/**
 * Read RGB values. Each value ranges from 0 to 1023
 * @param port
 * @return
 */
RGB ReadEV3ColorSensorRGB(int port);

void exitEV3ColorSensor(int port);

/* Utility functions */
int getEV3ColorSensorModeFromReadingLightMode(EV3ColorLightReadingMode);
int readEV3ColorSensorRawValue(int port, int mode);
void setEV3ColorSensorMode(int port, int mode);

#endif //EV3_API_EV3_COLOR_H
