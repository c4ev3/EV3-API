#ifndef EV3_API_EV3_COLOR_H
#define EV3_API_EV3_COLOR_H

#include "../ev3_sensor.h"

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
    ReflectedLight = COL_REFLECT,
    AmbientLight = COL_AMBIENT
} LightMode;

int ReadEV3ColorSensorLight(int sensorPort, LightMode mode);

Color ReadEV3ColorSensor(int sensorPort);


RGB ReadEV3ColorSensorRGB(int sensorPort);

#endif //EV3_API_EV3_COLOR_H
