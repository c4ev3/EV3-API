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
 * Example: setSensorMode(INPUT_1, COL_REFLECT)
 */
int setSensorMode(int sensorPort, int name);

/**
 * Set sensor mode for a all ports
 * Note: Can be only called once
 * Example: setAllSensorMode(TOUCH_PRESS, US_DIST_MM, NO_SEN, COL_COLOR)
 */
int setAllSensorMode(int name_1, int name_2, int name_3, int name_4);


/***********************************/
// Sensor Names
#define NO_SEN -1
//TOUCH
#define TOUCH_PRESS 1

//Light - Reflect
#define COL_REFLECT 2
//Light - Ambient
#define COL_AMBIENT 3
//Light - Color
#define COL_COLOR 4

//Gyro - CM
#define US_DIST_CM 5
//Gyro - MM
#define US_DIST_MM 6
//Gyro - IN
#define US_DIST_IN 7

//Gyro - angle
#define GYRO_ANG 8
//Gyro - rate
#define GYRO_RATE 9

//IR - Proximity
#define IR_PROX 10
//IR - Seek
#define IR_SEEK 11
//IR - Remote Control
#define IR_REMOTE 12

#endif // EV3SENSOR_H
