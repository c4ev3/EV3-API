/** \file my_robot.h
 * \brief Constants for robot definition
 *
 * my_robot.h contains declarations about structure of your robot.
 *
 * License:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * The Initial Developer of this code is John Hansen.
 * Portions  by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Red Team FLL (redteamfll_at_google.com)
 * \date 2020-08-20
 * \version 1
 * \note initial definitions RT21
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef my_robot_h
#define my_robot_h

#include "ev3_constants.h"

/** @addtogroup RobotConstants
 * @{
 */
#define ROBOT_COLORLEFT         IN_2    /*!< Color Sensor port Left Side */
#define ROBOT_COLORRIGHT        IN_3    /*!< Color Sensor port Left Side */
#define ROBOT_GYRO              IN_1    /*!< Gyro Sensor port  */
#define ROBOT_MOVE_MOTORLEFT    OUT_B   /*!< Move Motor port Left Side */
#define ROBOT_MOVE_MOTORRIGHT   OUT_C   /*!< Move Motor port Left Side */
#define ROBOT_ARM_A             OUT_A   /*!< ARM Motor port  */
#define ROBOT_ARM_B             OUT_D   /*!< ARM Motor port  */
#define ROBOT_WHEEL_DIAMETER    62.4    /*!< Wheel Diameter in mm  */
#define ROBOT_WIDTH             114.0   /*!< Distance between centre of wheels in mm  */
#define ROBOT_KFRICTION         1.0     /*!< Constant of stimated glide loss in the mat */
/** @} */  // end of RobotConstants group

/** @addtogroup PIDConstants
 * @{
 */
#define MOVE_GYRO_KP    2.0    /*!< Proportional constant Value for use in straight movement with Gyro */
#define MOVE_GYRO_KI    0.0    /*!< Integral constant Value for use in straight movement with Gyro remember multiply by Delta t if used*/
#define MOVE_GYRO_KD    0.0    /*!< Derivative constant Value for use in straight movement with Gyro remember divide by Delta t if used*/
#define MOVE_LIGHT_KP   1.0    /*!< Proportional constant Value for use in line follower or LG movement with Gyro */
#define MOVE_LIGHT_KI   1.0    /*!< Integral constant Value for use in line follower or LG movement with Gyro */
#define MOVE_LIGHT_KD   1.0    /*!< Derivative constant Value for use in line follower or LG movement with Gyro */
/** @} */  // end of PIDConstants group




#endif  // my_robot_h
#ifdef __cplusplus
}
#endif