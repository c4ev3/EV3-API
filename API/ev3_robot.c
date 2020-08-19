/*
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
 * The Initial Developer of this code is Simón Rodriguez Perez.
 * Portions created by Simón Rodriguez Perez are Copyright (C) 2014-2015 Simón Rodriguez Perez.
 * All Rights Reserved.
 *
 */
#include "ev3_robot.h"


typedef struct {
	uint8_t Pos_x;
	uint8_t Pos_y;
	short Head;
} ROBOT_POSE;

typedef struct {
	ROBOT_POSE Pose;
	int Width;
	float WheelDiameter;
	float WheelCircunference;
	int ColorLeft;
	int ColorRight;
	uint8_t MotorLeft;
	uint8_t MotorRight;
	uint8_t MotorDual;
	int Gyro;
	float Kfriction; // friction factor if robot glide in the competition table surface
} RobotGlobals;

RobotGlobals MyRobot;

typedef struct {
	float Kp;
	float Ki;
	float Kd;
} KPID;

KPID StraightPid;

void PoseInit(){
	MyRobot.Pose.Pos_x = 0;
	MyRobot.Pose.Pos_y = 0;
	MyRobot.Pose.Head = 0;
}

void SetKPID(float Kp, float Ki, float Kd){

	StraightPid.Kp = 2.0;
	StraightPid.Ki = 0.0;
	StraightPid.Kd = 0.0;
	if (Kp != 0) StraightPid.Kp = Kp;	
	if (Ki != 0) StraightPid.Ki = Ki;
	if (Kd != 0) StraightPid.Kd = Kd;

}

void RobotInit(int ColorLeft, int ColorRight, uint8_t MotorLeft, uint8_t MotorRight, int Gyro, bool Debug){
	
	PoseInit();
	SetKPID(0,0,0);

	MyRobot.WheelCircunference = MyRobot.WheelDiameter * M_PI;
	MyRobot.ColorLeft = ColorLeft;
	MyRobot.ColorRight = ColorRight;
	MyRobot.MotorLeft = MotorLeft;
	MyRobot.MotorRight = MotorRight;
	MyRobot.MotorDual = MotorLeft + MotorRight;
	MyRobot.Gyro = Gyro;
	MyRobot.Kfriction = 1.0;
	
	int reflectedLightLeft = ReadEV3ColorSensorReflectedLight(MyRobot.ColorLeft);
	int reflectedLightRight = ReadEV3ColorSensorReflectedLight(MyRobot.ColorRight);
	
	ResetEV3GyroSensor(MyRobot.Gyro, EV3GyroHardwareCommand0x88); // chack Gyro Veersion //
	ResetEV3GyroSensor(MyRobot.Gyro, EV3GyroHardwareReboot); // 2.5s, valid for all versions
	ResetEV3GyroSensor(MyRobot.Gyro, EV3GyroSoftwareOffset); /* chack for proper reset */
	wait(100);
	int angle = ReadEV3GyroSensorAngle(MyRobot.Gyro, EV3GyroNormalAngle);	

	ResetRotationCount(MyRobot.MotorDual);

	int rotationsLeft = MotorRotationCount(MyRobot.MotorLeft);
	int rotationsRight = MotorRotationCount(MyRobot.MotorRight);

	if (Debug){
 		LcdClean();
        LcdTextf(1, 0, LcdRowToY(1), "Color Left: %d", reflectedLightLeft);
		LcdTextf(1, 0, LcdRowToY(2), "Color Right: %d", reflectedLightRight);
        LcdTextf(1, 0, LcdRowToY(3), "Angle: %d", angle);
        LcdTextf(1, 0, LcdRowToY(4), "Rotations Left: %d", rotationsLeft);
        LcdTextf(1, 0, LcdRowToY(5), "Rotations Right: %d", rotationsRight);
        Wait(100);
	}
	
}

int Travelcm(int distance){

	return (int)(distance * MyRobot.Kfriction / MyRobot.WheelCircunference);

}

int StraightGyroDegrees(int distDegre, int angle, int speed, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(MyRobot.MotorLeft);
int rotationsRight = MotorRotationCount(MyRobot.MotorRight);
int angleNow = ReadEV3GyroSensorAngle(MyRobot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut; // control for motors

OnFwdSync(MyRobot.MotorDual, speed);
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(MyRobot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  StraightPid.Kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
	oldErrorD = error;
	
	/* 
	 * Proportional part
	 * (error ^  StraightPid.Kp)
	 * Integral part
	 * (errorI * StraightPid.Ki
	 * Derivative part
	 * (errorD * StraightPid.Kd )
	 *  Note; Delta t is considered in Ki, Kd values Ki*Delta t and Kd/Delta t
	 */

	// new power to motors
	// need to cheek max/min speed?
	OnFwdReg(MyRobot.MotorLeft, speed + uOut);
	OnFwdReg(MyRobot.MotorRight, speed - uOut);


	rotationsLeft = MotorRotationCount(MyRobot.MotorLeft);
	rotationsRight = MotorRotationCount(MyRobot.MotorRight);
	traveled = (rotationsLeft + rotationsRight) / 2; 	

}while (distDegre <= traveled);

}
