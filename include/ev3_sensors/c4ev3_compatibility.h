/** \file back_compatibility.h
 * \brief Back-compatible functions to read sensors like in the original c4ev3
 *
 */

#ifndef EV3_API_BACK_COMPATIBILITY_H
#define EV3_API_BACK_COMPATIBILITY_H

// Sensor Names
#include "ev3_sensors.h"

#define NO_SEN          -1  // No sensor connected
// Touchsenor
#define TOUCH_PRESS     1   // Press

// Light sensor
#define COL_REFLECT     2   // Reflect
#define COL_AMBIENT     3   // Ambient
#define COL_COLOR       4   // Color
#define COL_COLOR_RGB  23   // "Raw" Color in RGB

#define RGB_INT_GET_RED(x)   (((x) >> 16) & 0xFF)
#define RGB_INT_GET_GREEN(x) (((x) >>  8) & 0xFF)
#define RGB_INT_GET_BLUE(x)  (((x) >>  0) & 0xFF)

// Ultrasonic
#define US_DIST_CM      5   // Dist in cm
#define US_DIST_MM      6   // Dist in mm
#define US_DIST_IN      7   // Dist in inch

// Gyroskop
#define GYRO_ANG        8   // angle
#define GYRO_RATE       9   // rate

// Infrared
#define IR_PROX         10  // Proximity
#define IR_SEEK         11  // Seek
#define IR_REMOTE       12  // Remote Control

// NXT
#define NXT_TEMP_C      21  // Temperature in C
#define NXT_TEMP_F      22  // Temperature in F

// HiTechnic IR Seeker V2 Sensor
#define HT_DIR_DC       30
#define HT_DIR_AC       31
#define HT_DIR_DALL     32
#define HT_DIR_AALL     33

// HiTechnic Compass Sensor
#define HT_COMP         34

/**
 * Initializes the sensors. This function may be called only once at the beginning.
 * If one sensor initialization fails, the previous initialized sensors are
 * de-initialized and -1 is returned.
 * @param name_1
 * @param name_2
 * @param name_3
 * @param name_4
 * @return -1 in case of error
 */
int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4);

/**
 * Read value from the sensor connected to the specified port
 * @param sensorPort port to which the sensor is connected
 * @return value from the sensor. The range of value changes depending on the
 *          type of sensor
 */
int ReadSensor(int sensorPort);

/**
 * Set which channel of the ev3 IR sensor you want to read. Once you've set the
 * desidered channel, use ReadSensor to get the value
 * @param sensorPort port to which the sensor is connected
 * @param channel channel you want to read
 * @return 0
 */
int SetIRBeaconCH(int sensorPort, int channel);

#endif //EV3_API_BACK_COMPATIBILITY_H
