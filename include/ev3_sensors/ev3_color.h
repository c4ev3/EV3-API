#ifndef EV3_API_EV3_COLOR_H
#define EV3_API_EV3_COLOR_H

#include "ev3_sensors.h"
#include "limits.h"

// see http://docs.ev3dev.org/projects/lego-linux-drivers/en/ev3dev-buster/sensor_data.html#lego-ev3-color

#define EV3_COLOR_SENSOR_TYPE           29

#define EV3_COLOR_SENSOR_REFLECT_MODE      0
#define EV3_COLOR_SENSOR_AMBIENT_MODE      1
#define EV3_COLOR_SENSOR_COLOR_MODE        2
#define EV3_COLOR_SENSOR_RAW_REFLECT_MODE  3
#define EV3_COLOR_SENSOR_RAW_COLOR_MODE    4
#define EV3_COLOR_SENSOR_FACTORY_MODE      5
#define EV3_COLOR_SENSOR_DEFAULT_MODE      EV3_COLOR_SENSOR_REFLECT_MODE

/**
 * @brief EV3 Color Sensor (LEGO #45506)
 */
extern SensorHandler * EV3Color;

/**
 * @brief Colors that can be automatically recognized by the color sensor.
 */
typedef enum Color {
    //! Returned when the sensor read failed.
    ColorError  = INT_MIN,

    //! Cannot identify the color.
    ColorNone   = 0,

    //! Black color
    ColorBlack  = 1,

    //! Blue color
    ColorBlue   = 2,

    //! Green color
    ColorGreen  = 3,

    //! Yellow color
    ColorYellow = 4,

    //! Red color
    ColorRed    = 5,

    //! White color
    ColorWhite  = 6,

    //! Brown color
    ColorBrown  = 7,
} Color;

/**
 * @brief Raw red light reflection data.
 * This mode provides raw access to the measurements done by the sensor ADC.
 */
typedef struct RawReflect {
    /**
     * @brief Reflected light intensity.
     * This value represents the difference between LED on and LED off
     * ADC readings. It works similarly to COL-REFLECT readings.
     *
     * The values do not have a fixed range; expect something like 0-400.
     *
     * To normalize the values, you will need to find out the minimum
     * and maximum readings for your use case and then perform a
     * linear correction.
     *
     * @remark This value does not equal directly to the value that is sent
     *         by the sensor and that is documented on the ev3dev wiki.
     *         For easier use, here the suggested subtraction is
     *         performed immediately after the data is read.
     */
    int reflection;

    /**
     * @brief Reference background intensity.
     * This value represents the ADC reading when the LED is off.
     *
     * This value will be in range 0-1023; expect idle readings to be around 600.
     *
     * The sensor internally measures reflectivity by continuously flashing
     * the red LED on and off and measuring the current flowing through a bypass capacitor.
     *
     * It is therefore not possible to measure true ambient illumination
     * through this mode. However the value is used as a reference
     * for RawReflect::reflection. It can also be useful in observing
     * changes in the external illumination - the value will then
     * go either above or below its idle average.
     */
    int background;
} RawReflect;

/**
 * @brief Raw color reflection data.
 * This mode provides raw access to the measurements done by the sensor ADC.
 */
typedef struct RGB {
    /**
     * @brief Red reflected light intensity.
     * This value represents the difference between red LED on off
     * ADC readings. It works similarly to COL-REFLECT readings.
     *
     * The values do not have a fixed range; expect something like 0-400.
     *
     * To normalize the values, you will need to find out the minimum
     * and maximum readings for your use case and then perform a
     * linear correction.
     */
    int red;

    /**
     * @brief Green reflected light intensity.
     * This value represents the difference between green LED on and off
     * ADC readings. It works similarly to COL-REFLECT readings.
     *
     * The values do not have a fixed range; expect something like 0-400.
     *
     * To normalize the values, you will need to find out the minimum
     * and maximum readings for your use case and then perform a
     * linear correction.
     */
    int green;

    /**
     * @brief Blue reflected light intensity.
     * This value represents the difference between blue LED on and off
     * ADC readings. It works similarly to COL-REFLECT readings.
     *
     * The values do not have a fixed range; expect something like 0-400.
     *
     * To normalize the values, you will need to find out the minimum
     * and maximum readings for your use case and then perform a
     * linear correction.
     */
    int blue;

    /**
     * @brief Reference background intensity.
     * This value represents the ADC reading when the LED is off.
     *
     * This value will be in range 0-1023; expect idle readings to be around 600.
     *
     * The sensor internally measures reflectivity by continuously flashing
     * the color LEDs on and off and measuring the current flowing through a bypass capacitor.
     *
     * It is therefore not possible to measure true ambient illumination
     * through this mode. However the value is used internally as a reference
     * for the remaining channels. It can also be useful in observing
     * changes in the external illumination - the value will then
     * go either above or below its idle average.
     */
    int background;
} RGB;


/**
 * @brief Read the reflected light intensity using the COL-REFLECT mode.
 * @param port Port to which the sensor is connected
 * @return Relative reflectity (0-100) or INT_MIN on error
 */
int ReadEV3ColorSensorReflectedLight(int port);

/**
 * @brief Read the ambient light intensity using the COL-AMBIENT mode.
 * @param port Port to which the sensor is connected
 * @return Relative ambient illumination (0-100) or INT_MIN on error
 */
int ReadEV3ColorSensorAmbientLight(int port);

/**
 * @brief Read the reflected color using the COL-COLOR mode.
 * @param port Port to which the sensor is connected
 * @return Code of the measured color.
 *         On error, INT_MIN is returned.
 * @sa Color
 */
Color ReadEV3ColorSensorColor(int port);

/**
 * @brief Read the reflected color channels using the RGB-RAW mode.
 * @param port port to which the sensor is connected
 * @param rgb RGB values read from sensor (output variable)
 * @return On error, INT_MIN is returned, otherwise zero is returned.
 * @sa RGB
 */
int ReadEV3ColorSensorColorRGB(int port, RGB* rgb);

/**
 * @brief Read the raw reflected light intensity using the REF-RAW mode.
 * @param port Port to which the sensor is connected
 * @param refraw Where to store the internal sensor measurements.
 * @return On error, INT_MIN is returned, otherwise zero is returned.
 * @sa RawReflect
 */
int ReadEV3ColorSensorRawReflectedLight(int port, RawReflect *refraw);

#endif //EV3_API_EV3_COLOR_H
