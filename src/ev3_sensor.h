/*
* EV3 Sensor API
*
* Copyright (C) 2014 Carsten Zeiffert
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/
*
* ----------------------------------------------------------------------------
*
* \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
* \date 2015-02-28
* \version 2
* \note Correct data for Touch-, Sonar- and Lightsensor
*
*/
#ifndef EV3SENSOR_H
#define EV3SENSOR_H

#include <stdbool.h>
#include "../copied/lms2012/ev3_basictypes.h"

/**
 * Select channel for the Beacon control
 * Note: Can be changed while running
 * Example: setAllSensorMode(IN_2, BEACON_CH_1)
 */
int SetIRBeaconCH(int sensorPort, int channel);

/***********************************/
// Sensor Names
#define NO_SEN -1		// No sensor connected
//Touchsenor
#define TOUCH_PRESS 1	// Press

//Lightsensor
#define COL_REFLECT 2	// Reflect
#define COL_AMBIENT 3	// Ambient
#define COL_COLOR 4		// Color
#define COL_RGB 5       // Color in RGB values

//Ultrasonic
#define US_DIST_CM 6	// Dist in cm
#define US_DIST_MM 7	// Dist in mm
#define US_DIST_IN 8	// Dist in inch
#define US_LISTEN  9	// Presence of other sensors

//Gyroskop
#define GYRO_ANG 10		// angle
#define GYRO_RATE 11	// rate

//Infrared
#define IR_PROX 12		        // Proximity
#define IR_SEEK 13		        // Seek
#define IR_REMOTE 14	        // Remote Control

//NXT 
#define NXT_IR_SEEKER_DC 20 	// Infrared Seeker DC signal
#define NXT_IR_SEEKER_AC 21 	// Infrared Seeker AC signal
#define NXT_TEMP_C 22	    	// Temperature in C
#define NXT_TEMP_F 23	    	// Temperature in F
#define NXT_SOUND_DB 24     	// Sound pressure level - Flat weighting
#define NXT_SOUND_DBA 25    	// Sound pressure level - A weighting
#define NXT_COMPASS_COMPASS 26	// compass with values from 0 to 360
#define NXT_COMPASS_ANGLE 27	// compass with values from -180 to 180

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


void applySensorMode();


int GetSensorName (int port);


/**
 * Read the seek value for all the channels.
 * The returned array contains 8 integers, two values for each channel:
 * - position value (-25 - 25)
 * - raw value
 *
 * If no beacon is detected, raw value is 128.
 *
 * NOTE: This function returns a pointer to a static array. If you need to remember
 * old values, you'll need to copy them from the array before calling this function,
 * since the  array will be modified.
 * @param port
 * @return
 */
int* ReadIRSeekAllChannels(int port);


#endif // EV3SENSOR_H
