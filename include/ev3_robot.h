/** \file ev3_robot.h
 * \brief Naviugation funtions for a diffrential robot with the EV3 brick  for FLL Robots (POSE, Go , Gyro)
 *
 * ev3_robot.h contains declarations for the Robots movement functions.
 *
 * License:
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
 * The Initial Developer of this code is Red Team FL.
 * Portions created by Red Team FLL are Copyright (C) Red Team FLL.
 * 
 * 
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Red Team FLL (redteamfll_at_gmail.com)
 * \date 2020-08-15
 * \version 0.0.1
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef robot_h
#define robot_h

#include "inputs/ev3_input.h"
#include "sensors/ev3_sensors.h"
#include "ev3.h"
#include "math.h"
#ifndef M_PI
    #define M_PI		3.14159265358979323846	/* pi */
#endif


/**
 * @brief Initialize pose at postion (x,y) (0,0) and angle (0).
  *
 *  */
void PoseInit();


/**
 * @brief Initialize all robot variables, set POSE to (0,0,0),  * reset gyrosensor and reset tacho counters.
 * @param port Port to which the sensor is connected
 * @param mode How to measure the rate
 * @param ColorLeft Port to which the left color sensor is connected 
 * @param ColorRight Port to which the right color sensor is connected
 * @param MotorLeft Port to wich the traction Left motor is connected
 * @param MotorRight Port to wich the traction  right motor is connected
 * @param Gyro Port to which the gyro sensor is connected 
 * @param Debug If True print on the LCD screen the result of initialiation
 * @return N/A.
 */
void RobotInit(int ColorLeft, int ColorRight, uint8_t MotorLeft, uint8_t MotorRight, int Gyro, bool Debug);

/**
 * @brief Initialize PID for navigation with girosensor.
 * Default values set to (2,0,0) that's it's only proportional control.
 * @param Kp Proportional part constant, default value 2
 * @param Ki Integral part constant, default value 0
 * @param Kd Derivative part constant, default value 0 
 * @return N/A.
 */
void SetKPID(short Kp, short Ki,short Kd);

/**
 * @brief Convert mm into degrees.
 * Uses Kfriction, if it was calculated otherwise Kfriction must be equal to 1.0
 * @param Distance Distance to travel in mm 
 * @return distance to travel in degrees.
 */
int Travelcm(int Distance, uint8_t angle, int power);


#endif // ev3_robot_h

#ifdef __cplusplus
}
#endif
