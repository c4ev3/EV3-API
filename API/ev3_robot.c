/**
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
 * The Initial Developer of this code is Red Team FL.
 * Portions created by Red Team FLL are Copyright (C) Red Team FLL.
 * All Rights Reserved.
 *
 * \author Red Team FLL (redteamfll_at_gmail.com)
 * \date 2020-08-31
 * \version 0.1.0
 */
#include "ev3_robot.h"

typedef struct {
	short Pos_x;
	short Pos_y;
	int Head;
} ROBOT_POSE;

typedef struct {
	ROBOT_POSE Pose;
	int Width;
	float WheelDiameter;
	float WheelCircumference;
	float MmDegree;
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
KPID LightPid;

short GetPoseX(){
	return Robot.Pose.Pos_x;
}

short GetPoseY(){
	return Robot.Pose.Pos_y;
}

int GetPoseHead(){
	return Robot.Pose.Head;
}

void SetPoseX(short newX){
	Robot.Pose.Pos_x = newX;
}

void SetPoseY(short newY){
	Robot.Pose.Pos_y = newY;
}

int NormalizeHead(int Head){
	Head = Head % 360;
	if (Head > 180) Head -= 360;
	if (Head < -180) Head += 360;
	return Head;
}

void SetPoseHead(int newHead){
	if ((newHead > 180) || (newHead < -180)) newHead = NormalizeHead(newHead);
	Robot.Pose.Head = newHead;
}

void NewHeadPose(){

	SetPoseHead(ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE));	
}

void PoseInit(){

	Robot.Pose.Pos_x = 0;
	Robot.Pose.Pos_y = 0;
	Robot.Pose.Head = 0;
}

int TurnGyroRightAbs(int angle, int speed){
	
	int angleNow;
	int distance;
		
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
	distance = angle - angleNow;
	if (distance != 0 ) OutputTimeSync(Robot.MotorDual, speed, SPIN_RIGHT, 0);

	while (true){
		angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
		distance = angle - angleNow;
	
		if (distance == 0) {
			Off(Robot.MotorDual);
			return angleNow;
		}
		else if (distance < 0) {	
			OutputTimeSync(Robot.MotorDual, MIN_SPEED_SPIN, SPIN_LEFT, 0);
		}
		else if (distance < 30) {
			OutputTimeSync(Robot.MotorDual, MIN_SPEED_SPIN, SPIN_RIGHT, 0);
		}

	}
	return angleNow;
} // To do protection by time

int TurnGyroLeftAbs(int angle, int speed){
	
	int angleNow;
	int distance;

	
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
	distance = angleNow - angle;
	if (distance != 0 ) OutputTimeSync(Robot.MotorDual, speed, SPIN_LEFT, 0);

	while (true){
		angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
		distance = angleNow - angle;
	
		if (distance == 0) {
			Off(Robot.MotorDual);
			return angleNow;
		}
		else if (distance < 0) {	
			OutputTimeSync(Robot.MotorDual, MIN_SPEED_SPIN, SPIN_RIGHT, 0);
		}
		else if (distance < 30) {
			OutputTimeSync(Robot.MotorDual, MIN_SPEED_SPIN, SPIN_LEFT, 0);
		}

	}
	return angleNow;
}  //To do protection by time

int PivotGyroRightWheelAbs(int angle, int speed){
	
	int angleNow;
	int distance;
	bool backAndForward;
	int minSpeed = MIN_SPEED_PIVOT;
	
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
	distance = angle - angleNow;
	backAndForward = (distance >= 0);
	if (!backAndForward) {
	speed = -speed;
	minSpeed = -minSpeed; // if the movement is backwards then the motors must be reversed
	}
	
	if (distance != 0 ) OutputTimeSync(Robot.MotorDual, speed, PIVOT_RIGHT, 0);

	while (true){
		angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
		distance = angle - angleNow;
	
		if (distance == 0) {
			Off(Robot.MotorDual);
			return angleNow;
		}
		else if (abs(distance) < 0) {	
			OutputTimeSync(Robot.MotorDual, -minSpeed, PIVOT_RIGHT, 0);
		}
		else if (abs(distance) < 30) {
			OutputTimeSync(Robot.MotorDual, minSpeed, PIVOT_RIGHT, 0);
		}

	}
	return angleNow;
}

int PivotGyroLeftWheelAbs(int angle, int speed){
	
	int angleNow;
	int distance;
	bool backAndForward;
	int minSpeed = MIN_SPEED_PIVOT;
	
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
	distance = angleNow - angle;
	backAndForward = (distance >= 0);
	if (!backAndForward) {
	speed = -speed;
	minSpeed = -minSpeed;
	}
	
	if (distance != 0 ) OutputTimeSync(Robot.MotorDual, speed, PIVOT_LEFT, 0);

	while (true){
		angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
		distance = angleNow - angle;
	
		if (distance == 0) {
			Off(Robot.MotorDual);
			return angleNow;
		}
		else if (abs(distance) < 0) {	
			OutputTimeSync(Robot.MotorDual, -minSpeed, PIVOT_LEFT, 0);
		}
		else if (abs(distance) < 30) {
			OutputTimeSync(Robot.MotorDual, minSpeed, PIVOT_LEFT, 0);
		}

	}
	return angleNow;
}




void SetStraightPID(float Kp, float Ki, float Kd){

	StraightPid.Kp = Kp;	
	StraightPid.Ki = Ki;
	StraightPid.Kd = Kd;
}

void SetLightPID(float Kp, float Ki, float Kd){

	LightPid.Kp = Kp;	
	LightPid.Ki = Ki;
	LightPid.Kd = Kd;
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
	SetLightPID(0.3f, 0.0f, 0.0f); //Initialization of Light PID constants in only proportional mode, it can be override anytime after RobotInit

	Robot.Width = params->Width;
	
	Robot.WheelDiameter = params->WheelDiameter;
	Robot.WheelCircumference = params->WheelDiameter * M_PI;
	Robot.MmDegree = Robot.WheelCircumference / 360;

	Robot.ColorLeft = params->ColorLeftPort;
	Robot.ColorRight = params->ColorRightPort;	
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

	return (int)(mm * Robot.Kfriction / Robot.MmDegree);

}

int StraighbyGyroDegrees(int distDegree, int angle, int speed, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors


// OnFwdReg(Robot.MotorLeft, speed);
// OnFwdReg(Robot.MotorRight, speed);
OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
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


	OutputTimeSync(Robot.MotorDual, speed, uOut, 0);

	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

}while (distDegree <= traveled);

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}

int FollowLineDegrees(int distDegree, int lightsensor, int light, int speed, bool inOutSide, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int lightNow = ReadEV3ColorSensorReflectedLight(lightsensor);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors


OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	lightNow = ReadEV3ColorSensorReflectedLight(lightsensor);

	
	error = light - lightNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  LightPid.Kp) + (errorI * LightPid.Ki) + (errorD * LightPid.Kd ));
	oldErrorD = error;
	
	/* 
	 * Proportional part
	 * (error ^  LightPid.Kp)
	 * Integral part
	 * (errorI * LightPid.Ki
	 * Derivative part
	 * (errorD * LightPid.Kd )
	 *  Note; Delta t is considered in Ki, Kd values Ki*Delta t and Kd/Delta t
	 */

	// To DO : Check limits on DI
	if (inOutSide) uOut = -uOut; // In case of follow south/east border line then reverse

	OutputTimeSync(Robot.MotorDual, speed, uOut, 0);

	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

}while (distDegree <= traveled);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}
// To DO Use ramp function
return traveled;	 	

}


int StraighLAGDegrees(int distDegree, int lightsensor, int light, int angle, int speed, bool inOutSide, bool brake){
/*!< Based on LAGS Line Assisted Gyro Steering idea from Brickwolves Waring FLL Team*/

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int lightNow = ReadEV3ColorSensorReflectedLight(lightsensor);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int errorLight;
int errorGyro;
int error;
int uOut = 0; // control for motors


// OnFwdReg(Robot.MotorLeft, speed);
// OnFwdReg(Robot.MotorRight, speed);
OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	lightNow = ReadEV3ColorSensorReflectedLight(lightsensor);
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	errorLight = light - lightNow;
	if (inOutSide) errorLight = - errorLight; // For Proportional part due to light
	errorGyro = angle - angleNow;//
	error = errorGyro + (errorLight * LightPid.Kp); // angle compensation due linefollower
	uOut = (int)(error *  StraightPid.Kp);	
	/* 
	 * Only Proportional part
	 * 
	 */

	OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

}while (distDegree <= traveled);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
	// To DO Use ramp function
return traveled;
}