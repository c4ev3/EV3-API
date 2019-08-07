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

// Infrared Beacon Buttons
#define BEACON_CH_1 0
#define BEACON_CH_2 1
#define BEACON_CH_3 2
#define BEACON_CH_4 3
#define BEACON_OFF 			0
#define BEACON_UP_LEFT 		1
#define BEACON_DOWN_LEFT 	2
#define BEACON_UP_RIGHT 	3
#define BEACON_DOWN_RIGHT 	4
#define BEACON_UP			5
#define BEACON_DIAG_UP_LEFT		6
#define BEACON_DIAG_UP_RIGHT	7
#define BEACON_DOWN 		8
#define BEACON_ON			9
#define BEACON_LEFT 		10
#define BEACON_RIGHT 		11

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
int initializeAllSensors ();
void initializeBackCompatibilityIfNeeded ();
int readEV3IrSensorSeekForBackCompatibility (int port);

#endif //EV3_API_BACK_COMPATIBILITY_H
