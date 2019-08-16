#ifndef EV3_API_BACK_COMPATIBILITY_H
#define EV3_API_BACK_COMPATIBILITY_H

// Sensor Names
#include "ev3_sensors.h"

#define NO_SEN -1		// No sensor connected
// Touchsenor
#define TOUCH_PRESS 1	// Press

// Light sensor
#define COL_REFLECT 2	// Reflect
#define COL_AMBIENT 3	// Ambient
#define COL_COLOR 4		// Color

// Ultrasonic
#define US_DIST_CM 5	// Dist in cm
#define US_DIST_MM 6	// Dist in mm
#define US_DIST_IN 7	// Dist in inch

// Gyroskop
#define GYRO_ANG 8		// angle
#define GYRO_RATE 9		// rate

// Infrared
#define IR_PROX 10		// Proximity
#define IR_SEEK 11		// Seek
#define IR_REMOTE 12	// Remote Control

// NXT
#define NXT_IR_SEEKER 20 // Infrared Seeker
#define NXT_TEMP_C 21 	// Temperature in C
#define NXT_TEMP_F 22 	// Temperature in F

/**
 * Initializes the sensors. This function may be called only once at the beginning
 * @param name_1
 * @param name_2
 * @param name_3
 * @param name_4
 * @return
 */
int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4);

int ReadSensor(int sensorPort);

int SetIRBeaconCH(int sensorPort, int channel);

/* utility functions */
void initializeAllSensors ();
void initializeBackCompatibilityIfNeeded ();
int readEV3IrSensorSeekForBackCompatibility (int port);

#endif //EV3_API_BACK_COMPATIBILITY_H
