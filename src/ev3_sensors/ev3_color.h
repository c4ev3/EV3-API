#ifndef EV3_API_EV3_COLOR_H
#define EV3_API_EV3_COLOR_H

#include "../ev3_sensor.h"
#include "sensors.h"

extern SensorHandler * EV3Color;

typedef enum Color {
    None,
    Black,
    Blue,
    Green,
    Yellow,
    Red,
    White,
    Brown,
    Orange,
    Lime,
    Violet,
    Crimson,
    Purple,
    Grey
} Color;

typedef struct RGB {
    int red;
    int blue;
    int green;
} RGB;

typedef enum LightMode {
    ReflectedLight,
    AmbientLight
} LightMode;

bool initEV3ColorSensor(int port);

int ReadEV3ColorSensorLight(int port, LightMode mode);

Color ReadEV3ColorSensor(int port);

RGB ReadEV3ColorSensorRGB(int port);

void exitEV3ColorSensor(int port);

/* Utility functions */
int getEV3ColorLightSensorModeConstant (LightMode mode);
int readEV3ColorSensorRawValue(int port, int mode);

#endif //EV3_API_EV3_COLOR_H
