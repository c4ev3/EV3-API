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


/**
 * Reads light value
 * @param port port to which the sensor is connected
 * @param mode light mode
 * @return light value or -1 if error
 */
int ReadEV3ColorSensorLight(int port, EV3ColorLightReadingMode mode);

/**
 * Read the color or return -1 if the read fails
 * @param port port to which the sensor is connected
 * @return read color or -1 in case of error
 */
Color ReadEV3ColorSensor(int port);

/**
 * Read RGB values. Each value ranges from 0 to 1023
 * @param port port to which the sensor is connected
 * @param rgb RGB values read from sensor (output variable)
 * @return 0 is the read was successful, -1 otherwise
 */
int ReadEV3ColorSensorRGB(int port, RGB* rgb);

#endif //EV3_API_EV3_COLOR_H
