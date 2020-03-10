#ifndef EV3_API_EV3_ULTRASONIC_H
#define EV3_API_EV3_ULTRASONIC_H

#include <stdbool.h>
#include "ev3_sensors.h"

// see http://docs.ev3dev.org/projects/lego-linux-drivers/en/ev3dev-buster/sensor_data.html#lego-ev3-us

#define EV3_ULTRASONIC_SENSOR_TYPE              30

#define EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE  0
#define EV3_ULTRASONIC_SENSOR_DISTANCE_IN_MODE  1
#define EV3_ULTRASONIC_SENSOR_LISTEN_MODE       2
#define EV3_ULTRASONIC_SENSOR_SINGLE_MM_MODE    3
#define EV3_ULTRASONIC_SENSOR_SINGLE_IN_MODE    4
#define EV3_ULTRASONIC_SENSOR_DC_MM_MODE        5
#define EV3_ULTRASONIC_SENSOR_DC_IN_MODE        6
#define EV3_ULTRASONIC_SENSOR_DEFAULT_MODE      EV3_ULTRASONIC_SENSOR_DISTANCE_MM_MODE

#define EV3_ULTRASONIC_FIRE_COMMAND  0x11

/**
 * @brief EV3 Ultrasonic Sensor (LEGO #45504)
 */
extern SensorHandler * EV3Ultrasonic;

/**
 * @brief Units to measure the result in
 */
typedef enum EV3UltrasonicDistanceUnit {
    /**
     * @brief Return the distance in centimeters
     * (discard the decimal place of the *-MM measurement)
     * range: 0-255
     */
    EV3_ULTRASONIC_CM,

    /**
     * @brief Return the distance in millimetes (*-MM)
     * range: 0-255
     */
    EV3_ULTRASONIC_MM,

    /**
     * @brief Return the distance in inches (*-IN)
     * range: 0-1003
     */
    EV3_ULTRASONIC_IN,
} EV3UltrasonicDistanceUnit;

/**
 * @brief Returns the distance continually measured by the sensor (US-DIST-*)
 * @param port Port to which the sensor is connected
 * @param mode Wheter to read the distance in cm, mm or inches
 * @return measured value or INT_MIN in case of error
 */
int ReadEV3UltrasonicSensorDistance(int port, EV3UltrasonicDistanceUnit mode);

/**
 * @brief Returns the distance measured by the sensor by a single ping (US-SI-*)
 *
 * The distance will be measured only once after the mode is set.
 * To do the measurement again, either switch the sensor mode, or
 * call the FireEV3UltrasonicSensor() function.
 *
 * @param port Port to which the sensor is connected
 * @param mode Wheter to read the distance in cm, mm or inches
 * @return measured value or INT_MIN in case of error
 * @remark Beware, if you switch the sensor modes too often or
 *         trigger the ping too often, the sensor may lock up for 2-3 seconds.
 */
int ReadEV3UltrasonicSensorSingleDistance(int port, EV3UltrasonicDistanceUnit mode);

/**
 * @brief Returns the distance measured by the sensor in an unknown alternative way (US-DC-*)
 * @param port Port to which the sensor is connected
 * @param mode Wheter to read the distance in cm, mm or inches
 * @return measured value or INT_MIN in case of error
 * @remark It is not known how this mode is different from US-DIST-*.
 *         It is recommended to use US-DIST-* as that is
 *         the mode used in the stock lms2012 VM.
 */
int ReadEV3UltrasonicSensorDcDistance(int port, EV3UltrasonicDistanceUnit mode);

/**
 * @brief Makes the sensor send out a ping for the single measurement mode.
 * @param port Port to which the sensor is connected
 * @return 0 or INT_MIN in case of error
 */
int FireEV3UltrasonicSensor(int port);

/**
 * @brief Tries to find another ultrasonic sensor running in distance mode (US-LISTEN)
 * @param port Port to which the sensor is connected
 * @return true if there exists another ultrasonic sensor reading in distance mode,
 *         false if not and finally INT_MIN in case of error
 */
int ReadEV3UltrasonicSensorListen(int port);

#endif //EV3_API_EV3_ULTRASONIC_H
