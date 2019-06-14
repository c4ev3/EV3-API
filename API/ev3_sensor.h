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

/**
 * Initializes sensor I/O.
 */
int SensorsInit();

/**
 * Check if sensors are initialized.
 */
bool SensorsInitialized();

/**
 * Unmap sensors and close fds.
 */
bool SensorsExit();

/**
 * Reads the sensor value from a specific port.
 * Example: readSensor(INPUT_1)
 * Returns a raw sensor value.
 */
int ReadSensor(int sensorPort);

/**
 * Returns pointer to the current sensor value.
 * The sensor value may be up to 32 bytes long - this function
 * can be used to access it if readSensor() is inadequate.
 */
void* ReadSensorData(int sensorPort);

/**
 * Doesn't do anything. Don't use
 */
__attribute__((deprecated)) int SetSensorMode(int sensorPort, int name);

/**
 * Set sensor mode for a all ports
 * Note: Can be only called once
 * Example: setAllSensorMode(TOUCH_PRESS, US_DIST_MM, NO_SEN, COL_COLOR)
 */
int SetAllSensorMode(int name_1, int name_2, int name_3, int name_4);

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
#define NXT_IR_SEEKER_DC 20 // Infrared Seeker DC signal
#define NXT_IR_SEEKER_AC 21 // Infrared Seeker AC signal
#define NXT_TEMP_C 22	    // Temperature in C
#define NXT_TEMP_F 23	    // Temperature in F
#define NXT_SOUND_DB 24     // Sound pressure level - Flat weighting
#define NXT_SOUND_DBA 25    // Sound pressure level - A weighting
#define NXT_COMPASS 26

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
 * COMPATIBILITY FUNCTION NAMES
 */

/*!
 * @deprecated
*/
#define initSensors() SensorsInit()

/*!
 * @deprecated
*/
#define InitSensors() SensorsInit()

/*!
 * @deprecated
*/
#define readSensor(_sensorPort) ReadSensor(_sensorPort)

/*!
 * @deprecated
*/
#define readSensorData(_sensorPort) ReadSensorData(_sensorPort)

/*!
 * @deprecated
*/
#define setAllSensorMode(_name_1, _name_2, _name_3, _name_4) SetAllSensorMode(_name_1, _name_2, _name_3, _name_4)

/*!
 * @deprecated
*/
#define setIRBeaconCH(_sensorPort, _channel) SetIRBeaconCH(_sensorPort, _channel)


int GetSensorName (int port);

void ResetGyroSensor (int port);

/**
 * @param rgb
 * @return red (0-255)
 */
int GetRFromRGB(int rgb);

/**
 * @param rgb
 * @return green (0-255)
 */
int GetGFromRGB(int rgb);

/**
 * @param rgb
 * @return blue (0-255)
 */
int GetBFromRGB(int rgb);


#endif // EV3SENSOR_H
