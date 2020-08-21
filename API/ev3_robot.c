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
// #include "my_robot.h"  Deprecated /*  Definition file for robot structure */

typedef struct {
	uint8_t Pos_x;
	uint8_t Pos_y;
	short Head;
} ROBOT_POSE;

typedef struct {
	ROBOT_POSE Pose;
	int Width;
	float WheelDiameter;
	float WheelCircumference;
	int ColorLeft;
	int ColorRight;
	int ColorCenter;
    int ColorAux;
	uint8_t MotorLeft;
	uint8_t MotorRight;
	uint8_t MotorDual;
    uint8_t ArmA;
    uint8_t ArmB;
	int Gyro;
	int GyroAux;
	float Kfriction; // friction factor if robot glide in the competition table surface
} RobotGlobals;


RobotGlobals Robot;

typedef struct {
	float Kp;
	float Ki;
	float Kd;
} KPID;

KPID StraightPid;

void PoseInit(){

	Robot.Pose.Pos_x = 0;
	Robot.Pose.Pos_y = 0;
	Robot.Pose.Head = 0;
}

void SetStraightPID(float Kp, float Ki, float Kd){

	StraightPid.Kp = Kp;	
	StraightPid.Ki = Ki;
	StraightPid.Kd = Kd;
}


int ResetGyroSensor(int port){

	ResetEV3GyroSensor(port, EV3GyroHardwareCommand0x88); // check Gyro Veersion //
	ResetEV3GyroSensor(port, EV3GyroHardwareReboot); // 2.5s, valid for all versions
	ResetEV3GyroSensor(port, EV3GyroSoftwareOffset); /* check for proper reset */
	return (ReadEV3GyroSensorAngle(port, EV3GyroNormalAngle));	
}

void RobotInit(ROBOT_PARAMS *params, bool Debug){

	int angle = 0;
	int angleaux = 0;

	PoseInit();
	
	SetStraightPID(2.0f, 0.0f, 0.0f); //Initialization of Straight PID constants in only proportional mode, it can be override anytime after RobotInit

	Robot.Width = params->Width;
	
	Robot.WheelDiameter = params->WheelDiameter;
	Robot.WheelCircumference = params->WheelDiameter * M_PI;

	Robot.ColorLeft = params->ColorLeftPort;
	Robot.ColorLeft = params->ColorRightPort;	
	Robot.ColorCenter = params->ColorCenterPort;	
	Robot.ColorAux = params->ColorAuxPort;

	Robot.MotorLeft = params->LeftMotorPort;
	Robot.MotorRight = params->RightMotorPort;
	Robot.MotorDual = params->LeftMotorPort + params->RightMotorPort;
	
	Robot.Gyro = params->GyroPort;
	Robot.GyroAux = params->GyroAuxPort;
	
	Robot.Kfriction = params->Kfriction;
	
	if (Robot.Gyro > NA){ 
	angle = ResetGyroSensor(Robot.Gyro);	
	}
	if (Robot.GyroAux > NA){ 
	angleaux = ResetGyroSensor(Robot.GyroAux);	
	}


	ResetRotationCount(Robot.MotorDual);

	int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	int rotationsRight = MotorRotationCount(Robot.MotorRight);

	if (Debug){
 		LcdClean();
		if (Robot.ColorLeft > NA){ 
			int reflectedLightLeft = ReadEV3ColorSensorReflectedLight(Robot.ColorLeft);	
        	LcdTextf(1, 0, LcdRowToY(1), "Color Left: %d", reflectedLightLeft);
		}
		if (Robot.ColorRight > NA){ 
			int reflectedLightRight = ReadEV3ColorSensorReflectedLight(Robot.ColorRight);
			LcdTextf(1, 0, LcdRowToY(2), "Color Right: %d", reflectedLightRight);
		}
		if (Robot.ColorCenter > NA){ 
			int reflectedLightCenter = ReadEV3ColorSensorReflectedLight(Robot.ColorCenter);
			LcdTextf(1, 0, LcdRowToY(3), "Color Center: %d", reflectedLightCenter);
		}
		if (Robot.Gyro > NA){ 
        	LcdTextf(1, 0, LcdRowToY(4), "Angle: %d", angle);
		}
		if (Robot.GyroAux > NA){ 
			LcdTextf(1, 0, LcdRowToY(4), "Angle Aux: %d", angleaux);
		}
        LcdTextf(1, 0, LcdRowToY(6), "Rotations Left: %d", rotationsLeft);
        LcdTextf(1, 0, LcdRowToY(7), "Rotations Right: %d", rotationsRight);
    
	}
	
}


int CalculateTravelDegrees(int mm){

	return (int)(mm * Robot.Kfriction / Robot.WheelCircumference);

}

int StraightGyroDegrees(int distDegree, int angle, int speed, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut; // control for motors

/* OnFwdSync(Robot.MotorDual, speed);*/
OnFwdReg(Robot.MotorLeft, speed);
OnFwdReg(Robot.MotorRight, speed);
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
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
	// To DO : Check limits on DI
	OnFwdReg(Robot.MotorLeft, speed + uOut);
	OnFwdReg(Robot.MotorRight, speed - uOut);

	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

}while (distDegree <= traveled);

if (brake) {
	Off(Robot.MotorLeft);
	Off(Robot.MotorRight);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}
