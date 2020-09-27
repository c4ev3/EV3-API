/** \file ev3_robot.h
 * \brief Naviugation funtions for a differential robot with the EV3 brick  for FLL Robots (POSE, Go , Gyro)
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
 * \date 2020-08-31
 * \version 0.1.0
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
// #include "my_robot.h" deprecated use of configuration file use ROBOT_PARAMS instead

#ifndef M_PI
    #define M_PI		3.14159265358979323846	/* pi */
#endif

#define NA  -1 /*!< used when a param is not applicable */

#define SPIN_RIGHT	  200	 /*!< Turn Value to spin the robot to the right -> left wheel forward right wheel backwards */
#define SPIN_LEFT	 -200	 /*!< Turn Value to spin the robot to the left -> left wheel backwards right wheel forward */
#define PIVOT_RIGHT	  100	 /*!< Turn Value to pivot the robot over the right wheel -> right wheel stopped */
#define PIVOT_LEFT	 -100	 /*!< Turn Value to pivot the robot over the right wheel -> right wheel stopped */

#define MIN_SPEED_SPIN	  6	 /*!< Minimun velocity for spin turn under 30º */
#define MIN_SPEED_PIVOT  10  /*!< Minimun velocity for pivot turn under 30º */

#define BLACK_DETECTED		12   /*!< Threshold for Black < BLACK_DETECTED line color */
#define WHITE_DETECTED		80   /*!< Threshold for White > WHITE_DETECTED line color */

#define NO_LINE_DETECTED		0   /*!< No line  found */
#define LINE_WHITE_DETECTED		1   /*!< White detected, maybe a line */
#define LINE_DETECTED			2   /*!< If Black is detected and previous state is LINE_WHITE_DETECTED */

#define MAXLEVEL				2	/*!< Two levels of menu, one for runs, other for utils or more runs if necessary */
#define NUMBUTTONS				4	/*!< Four buttons are used in botton menu, up, down right and left */
#define MENU_UP					0   /*!< index for button up in Button Style Menu */
#define MENU_RIGHT				1   /*!< index for button right in Button Style Menu */
#define MENU_DOWN				2   /*!< index for button down in Button Style Menu */
#define MENU_LEFT				3   /*!< index for button left in Button Style Menu */

/**
 * @brief Struct which contains information about mechanical design of robot.
 * @typedef This struct must be used to define and initialize the robot
 * 
 *  */
typedef struct {
	int Width;              /*!< Width of the robot measure from centers of wheels */
	float WheelDiameter;    /*!< Diameter of wheels of the robot */
	int ColorLeftPort;          /*!< Port used by Color Sensor in the left of robot, if nout used must be initializated to NA (-1) */
	int ColorRightPort;         /*!< Port used by Color Sensor in the right of robot, if nout used must be initializated to NA (-1) */
	int ColorCenterPort;        /*!< Port used by Color Sensor in the center of robot, for three sensor follow liners ans similars, if nout used must be initializated to NA (-1) */
    int ColorAuxPort;           /*!< Port used by Color Sensor Auxiliar, if it is used to select missions or..., if nout used must be initializated to NA (-1) */
	uint8_t LeftMotorPort;      /*!< Port used by left drive motor of robot */
	uint8_t RightMotorPort;     /*!< Port used by right drive motor of robot */
    uint8_t ArmAMotorPort;           /*!< Port used by arm motor A, if nout used must be initializated to NA (-1) */
    uint8_t ArmBMotorPort;           /*!< Port used by arm motor B, if nout used must be initializated to NA (-1) */
	int GyroPort;               /*!< Port used by Gyro Sensor, for drive straigth with gyro, if nout used must be initializated to NA (-1) */
	int GyroAuxPort;            /*!< Port used by auxiliary Gyro Sensor, for backup, tilt measurement.. if not used must be initializated to NA (-1) */
	float Kfriction;        /*!< friction factor if robot glide in the competition table surface, if not used must be initializated to 1.0f */
} ROBOT_PARAMS;

/**
 * @brief Returns x position of POSE register.
 * @return Pos_x Value
 *
 *  */
short GetPoseX();

/**
 * @brief Returns y position of POSE register.
 * @return Pos_y Value
 *
 *  */
short GetPoseY();

/**
 * @brief Returns head position of POSE register.
 * @return Head Value
 *
 *  */
int GetPoseHead();

/**
 * @brief Set x value of POSE register.
 * @param newX New value of Pos_x
 *
 *  */
void SetPoseX(short newX);

/**
 * @brief Set y value of POSE register.
 * @param newY New value of Pos_y
 *
 *  */
void SetPoseY(short newY);

/**
 * @brief Normalize value of any angle to range [-180, 180] degrees
 *
 *  */
int NormalizeHead(int Head);

/**
 * @brief Set Head value of POSE register.
 * @param newHead New value of Head
 *
 *  */
void SetPoseHead(int newHead);

/**
 * @brief Set Head value of POSE register reading the value of the Gyro Sensor.
 *
 *  */
void NewHeadPose();

/**
 * @brief Initialize pose at position (x,y) (0,0) and angle (0).
 *
 *  */
void PoseInit();

/**
 * @brief Rotate the robot on the central axis to the right (more degrees than current)
 * at constant speed, until the target angle is less than 30 degrees, then the speed is reduced to MIN_SPEED_SPIN
 * @param angle Target angle
 * @param speed  Max turning speed ( in %)
 * @return last angle measured
 *
 *  */
int TurnGyroRightAbs(int angle, int speed);

/**
 * @brief Rotate the robot on the central axis to the left (lest degrees than current)
 * at constant speed, until the target angle is less than 30 degrees, then the speed is reduced to MIN_SPEED_SPIN
 * @param angle Target angle
 * @param speed  Max turning speed ( in %)
 * @return last angle measured
 *
 *  */
int TurnGyroLeftAbs(int angle, int speed);

/**
 * @brief Pivot the robot on the right wheel
 * at constant speed, until the target angle is less than 30 degrees, then the speed is reduced to MIN_SPEED_PIVOT
 * @param angle Target angle (can be positive (forward) or negative (backward)
 * @param speed  Max turning speed ( in %)
 * @return last angle measured
 *
 *  */
int PivotGyroRightWheelAbs(int angle, int speed);

/**
 * @brief Pivot the robot on the left wheel
 * at constant speed, until the target angle is less than 30 degrees, then the speed is reduced to MIN_SPEED_PIVOT
 * @param angle Target angle (can be positive (forward) or negative (backward)
 * @param speed  Max turning speed ( in %)
 * @return last angle measured
 *
 *  */
int PivotGyroLeftWheelAbs(int angle, int speed);

/**
 * @brief Set the constant values for PID using GyroSensor
 * @param Kp Proportional constant
 * @param Ki Integral constant
 * @param Kd Derivative constant
 *
 *  */
void SetStraightPID(float Kp, float Ki, float Kd);

/**
 * @brief Set the constant values for PID used for LineFollover
 * @param Kp Proportional constant
 * @param Ki Integral constant
 * @param Kd Derivative constant
 *
 *  */
void SetLightPID(float Kp, float Ki, float Kd);

/**
 * @brief Reset the Gyro Sensor Hw & Sw, please remember that the robot must not move during the restart.
 * Use robot definition file my_robot.h
 * @param port Port where the gyro sensor is connected
 * @return N/A.
 */
int ResetGyroSensor(int port);


/**
 * @brief Initialize all robot variables, set POSE to (0,0,0),  * reset gyrosensor and reset tacho counters.
 * Use robot definition file my_robot.h
 * @param params pointer to struct which contains main design params of the robot 
 * @param Debug If True print on the LCD screen the result of initialiation
 * @return N/A.
 */
void RobotInit(ROBOT_PARAMS *params, bool Debug);

/**
 * @brief Convert mm into degrees.
 * Uses Kfriction, if it was calculated otherwise Kfriction must be equal to 1.0
 * @param mm Distance to travel in mm 
 * @return distance to travel in degrees.
 */
int CalculateTravelDegrees(int mm);

/**
 * @brief Navigate a distance in degrees
 * 
 * @param distDegree Distance to travel in degrees
 * @param angle Head of the robot to go straight
 * @param speed Speed of the robot 0 - 100
 * @param brake if true stop the motors at the end
 * @return distance traveled in degrees.
 */
int StraightbyGyroDegrees(int distDegree, int angle, int speed, bool brake);

/**
 * @brief Navigate a distance in degrees following a line border
 * 
 * @param distDegree Distance to travel in degrees
 * @param lightSensor Light Sensor Port used
 * @param light light threshold for border detection
 * @param speed Speed of the robot 0 - 100
 * @param inOutSide Which border of the line is followed
 * @param brake if true stop the motors at the end
 * @return distance traveled in degrees.
 */
int FollowLineDegrees(int distDegree, int lightSensor, int light, int speed, bool inOutSide, bool brake);

/**
 * @brief Navigate a distance in degrees with gyro and line follower, the gyro PID is tuned with line follower for more precision
 * Initial Development in EV3-G by Brickwolves Waring FLL Team
 * @param distDegree Distance to travel in degrees
 * @param lightsensor Light Sensor Port used
 * @param light light threshold for border detection
 * @param angle Head of the robot to go straight
 * @param speed Speed of the robot 0 - 100
 * @param inOutSide Which border of the line is followed
 * @param brake if true stop the motors at the end
 * @return distance traveled in degrees.
 */
int StraighLAGDegrees(int distDegree, int lightSensor, int light, int angle, int speed, bool inOutSide, bool brake);

/**
 * @brief Navigate straigh with gyro until a line is found, typical lines in FLL are a sandwich white-black-line
 * The robot stop at the white to black border detection
 * 
 * @param lightSensor Light Sensor Port used
 * @param angle Head of the robot to go straight
 * @param speed Speed of the robot 0 - 100
 * @param brake if true stop the motors at the end
 * @return distance traveled in degrees.
 */
int StraighbyGyroDegreesToLine(int lightSensor, int angle, int speed, bool brake);

/**
 * @brief Navigate straigh with rotations control until a line is found, typical lines in FLL are a sandwich white-black-line
 * The robot stop at the white to black border detection
 * 
 * @param lightSensor Light Sensor Port used
 * @param speed Speed of the robot 0 - 100
 * @param brake if true stop the motors at the end
 * @return distance traveled in degrees.
 */
int StraighbyDegreesToLine(int lightSensor, int speed, bool brake);


/**
 * @brief Assign keys, text and functions for use in a button style menu for launching in FLL competition. Each run must be defined in a function
 * and assigned to a key, up, right, down or left. The center key switchs menus.
 * The menu by default has two levels, 0 and 1, because mostly common use is four runs, the other four runs can be used for tools or test purposes
 * Each level must be initilzated properly
 * 
 * @param level Level of menu to be initializated
 * @param up pointer to the function that is called by pressing the UP key
 * @param right pointer to the function that is called by pressing the RIGHT key
 * @param down pointer to the function that is called by pressing the DOWN key
 * @param left pointer to the function that is called by pressing the LEFT key
 * @param upText Text displayed in the menu for the key UP
 * @param rightText Text displayed in the menu for the key RIGHT
 * @param downText Text displayed in the menu for the key DOWN
 * @param leftText Text displayed in the menu for the key LEFT
 * @return N/A
 */
void CreateMenuKeys(int level, int (*up)(), int (*right)(), int (*down)(), int (left)(), char  *upText,char  *rightText,char  *downText,char  *leftText);

/**
 * @brief Launch the buttons style menu. Each direction key must be assigned to a run or other utility function. The CENTER key switchs between the levels of the menu.
 	each function must return an int value
 * Example of use:
 *   CreateMenuKeys(0, Run1, Run2, Run3, Run4, "Run1","Run2","Run3","Run4");
 *   CreateMenuKeys(1, TestLight, ResetGyro, InitRobot, Calibrate, "Test_lights","Reset_Gyro","Init","Calibrate");
 *   MenuButtons();
 * 
 * @param N/A
 * @return N/A
 */
void MenuButtons ();
 
#endif // ev3_robot_h

#ifdef __cplusplus
}
#endif
