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

/**
 * Initializes sensor I/O.
 */
int initSensors();

/**
 * Reads the sensor value from a specific port.
 * Example: readSensor(INPUT_1)
 * Returns a raw sensor value.
 */
int readSensor(int sensorPort);

/**
 * Returns pointer to the current sensor value.
 * The sensor value may be up to 32 bytes long - this function
 * can be used to access it if readSensor() is inadequate.
 */
void* readSensorData(int sensorPort);

/**
 * Set sensor mode for a specific port.
 * Example: setSensorMode(INPUT_1, EV3_IR_MODE_REMOTE)
 */
int setSensorMode(int sensorPort, int name);

/**
 * Set sensor mode for a all ports
 * 
 */
int setAllSensorMode(int name_1, int name_2, int name_3, int name_4);

// Sensor ports
#define INPUT_1 0
#define INPUT_2 1
#define INPUT_3 2
#define INPUT_4 3

// EV3 color sensor modes
#define COLOR_MODE_REFLECT 0
#define COLOR_MODE_AMBIENT 1
#define COLOR_MODE_COLOR 2

// EV3 IR sensor modes
#define EV3_IR_MODE_PROXIMITY 0
#define EV3_IR_MODE_SEEK 1
#define EV3_IR_MODE_REMOTE 2
#define EV3_IR_MODE_REMOTE_A 3
#define EV3_IR_MODE_S_ALT 4
#define EV3_IR_MODE_CAL 5

/***********************************/
// define of Sensor setup
#define NO_SEN -1
//TOUCH
#define TOUCH_PRESS 1
#define TOUCH_PRESS_CON 121
#define TOUCH_PRESS_TYPE 16
#define TOUCH_PRESS_MODE 0

//Light - Reflect
#define COL_REFLECT 2
#define COL_REFLECT_CON 122
#define COL_REFLECT_TYPE 29
#define COL_REFLECT_MODE 0
//Light - Ambient
#define COL_AMBIENT 3
#define COL_AMBIENT_CON 122
#define COL_AMBIENT_TYPE 29
#define COL_AMBIENT_MODE 1
//Light - Color
#define COL_COLOR 4
#define COL_COLOR_CON 122
#define COL_COLOR_TYPE 29
#define COL_COLOR_MODE 2

//Gyro - CM
#define US_DIST_CM 5
#define US_DIST_CM_CON 122
#define US_DIST_CM_TYPE 30
#define US_DIST_CM_MODE 0
//Gyro - MM
#define US_DIST_MM 6
#define US_DIST_MM_CON 122
#define US_DIST_MM_TYPE 30
#define US_DIST_MM_MODE 0
//Gyro - IN
#define US_DIST_IN 7
#define US_DIST_IN_CON 122
#define US_DIST_IN_TYPE 30
#define US_DIST_IN_MODE 0

//Gyro - angle
#define GYRO_ANG 8
#define GYRO_ANG_CON 122
#define GYRO_ANG_TYPE 32
#define GYRO_ANG_MODE 0
//Gyro - rate
#define GYRO_RATE 9
#define GYRO_RATE_CON 122
#define GYRO_RATE_TYPE 32
#define GYRO_RATE_MODE 1

//IR - Proximity
#define IR_PROX 10 
#define IR_PROX_CON  122
#define IR_PROX_TYPE 33
#define IR_PROX_MODE 0
//IR - Seek
#define IR_SEEK 10 
#define IR_SEEK_CON  122
#define IR_SEEK_TYPE 33
#define IR_SEEK_MODE 1
//IR - Remote Control
#define IR_REMOTE 10 
#define IR_REMOTE_CON  122
#define IR_REMOTE_TYPE 33
#define IR_REMOTE_MODE 2


/*typedef   struct
{
  DATA8   TOUCH[3] 		= {121, 16, 0};
  DATA8   US_DIST_CM[3] = {122, 30, 0};
}
SENSOR_SETUP;*/

#endif // EV3SENSOR_H
