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
	float GearMotor;
	float GearWheel;
	int Direction;
} RobotGlobals;


RobotGlobals Robot;

typedef struct {
	float Kp;
	float Ki;
	float Kd;
} KPID;

KPID StraightPid;
KPID LightPid;


typedef struct {
	char *name;
	int (*function)();
} MenuEntry;

MenuEntry MenuButton[MAXLEVEL][NUMBUTTONS];

int degreesStrip;


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
	//Robot.MmDegree = Robot.WheelCircumference / 360;

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

	if (params->GearMotor > 1)
	Robot.GearMotor = params->GearMotor;
	else Robot.GearMotor = 1;

	if (params->GearWheel > 1)
	Robot.GearWheel = params->GearWheel;
	else Robot.GearWheel = 1;

	if (params->Direction == REVERSE)
	Robot.Direction = REVERSE;
	else  Robot.Direction = NORMAL;

	Robot.MmDegree = (Robot.WheelCircumference * (Robot.GearMotor / Robot.GearWheel)) / 360 ;

	ResetRotationCount(Robot.MotorDual);

	int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	int rotationsRight = MotorRotationCount(Robot.MotorRight);

	degreesStrip = CalculateStripDegrees();

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

int CalculateStripDegrees(){
return (int)(FLLLINE * 1.5 * Robot.Kfriction / Robot.MmDegree);	
}


int testStraightbyGyroDegrees(int distDegree, int angle, int speed, bool brake){

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
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 
OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
LcdTextf(1, 0, LcdRowToY(3), "hasta...%d desde %d",distDegree, traveled);
//Wait(2000);
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
LcdTextf(1, 0, LcdRowToY(5), "llevo...%d", traveled);
}while (distDegree >= traveled);

//if (brake) {
//	Off(Robot.MotorDual);
//}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}

int StraightbyGyroDegreesWithRamps(int distDegree, int angle, int speedTravel, int speedInit, int speedEnd, bool resetCounters, bool brake){

int traveled ; //= 0;
int rotationsLeft ;//= MotorRotationCount(Robot.MotorLeft);
int rotationsRight ;//= MotorRotationCount(Robot.MotorRight);
int angleNow ; //= ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part
int speed = speedInit;
int uOut = 0; // control for motors
float kAccel;
float kDaccel;
int initTravel; //Degrees traveled in accelaration ramp state


int relativeTraveled;
int remainTravel;


kAccel = (int)((speedTravel - speedInit) / RAMP_UP);
kDaccel = (int)((speedTravel - speedEnd) / RAMP_DOWN);

if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
initTravel = (int)((rotationsLeft + rotationsRight) / 2); 
traveled = initTravel;


//RotateMotorRampNoWait()
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  StraightPid.Kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
	oldErrorD = error;
	
	relativeTraveled = traveled - initTravel;
	remainTravel = distDegree - traveled;

	if (relativeTraveled < RAMP_UP) speed = speedInit + (int)(relativeTraveled*kAccel);

	else if (remainTravel <= RAMP_DOWN) speed = speedEnd + (int)(remainTravel*kDaccel);
	else speed = speedTravel;
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

}while (traveled <= distDegree);

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}


int StraightbyGyroDegreesWithAccel(int distDegree, int angle, int speedTravel, int speedInit, bool resetCounters, bool brake){

int traveled ; //= 0;
int rotationsLeft ;//= MotorRotationCount(Robot.MotorLeft);
int rotationsRight ;//= MotorRotationCount(Robot.MotorRight);
int angleNow ; //= ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part
int speed = speedInit;
int uOut = 0; // control for motors
int kAccel;
int ramp; //Degrees traveled in ramp state
int relativeTraveled;


kAccel = (int)((speedTravel - speedInit) / RAMP_UP);

if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
ramp = (int)((rotationsLeft + rotationsRight) / 2); 
traveled = ramp;
//RotateMotorRampNoWait()
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  StraightPid.Kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
	oldErrorD = error;
	
	relativeTraveled = traveled - ramp;
	if (relativeTraveled < RAMP_UP) {
		speed = speedInit + (relativeTraveled*kAccel);
	} else speed = speedTravel;
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

}while (traveled <= distDegree);

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}

int StraightbyGyroDegrees(int distDegree, int angle, int speed, bool resetCounters, bool brake){

int traveled ; //= 0;
int rotationsLeft ;//= MotorRotationCount(Robot.MotorLeft);
int rotationsRight ;//= MotorRotationCount(Robot.MotorRight);
int angleNow ; //= ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors


if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
//RotateMotorRampNoWait()
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

}while (traveled <= distDegree);

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

}while (traveled <= distDegree);

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

}while (traveled <= distDegree);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
	// To DO Use ramp function
return traveled;
}

int StraighbyGyroDegreesToLine(int lightSensor, int angle, int speed, bool brake){
/*!< Straigh move controlled by gyro and finished on line detection*/

int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors
int state = NO_LINE_DETECTED; // state for line transition detection, white = 1, black and previous white = 2, other 0

int lightNow ; //input from light sensor in light reflected mode

int stripBorder=0; //distance from white detected



do {
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
	lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);

/*	LcdClean();
	LcdTextf(1, 0, LcdRowToY(2), "Luz...%d",lightNow);
	LcdTextf(1, 0, LcdRowToY(4), "Estado...%d %d",state, rotationsRight);
	LcdTextf(1, 0, LcdRowToY(5), "stri...%d %d",stripe,DegreesStrip);		
**/
	switch (state){

		case LINE_WHITE_DETECTED: if (lightNow <= BLACK_DETECTED) {
									//Off(Robot.MotorDual);
									state = LINE_DETECTED;
									} else if ((rotationsRight-stripBorder) > degreesStrip) {
											state = NO_LINE_DETECTED;
											}
							break;
		
		case NO_LINE_DETECTED: if (lightNow >= WHITE_DETECTED) {
									stripBorder = rotationsRight;
									state = LINE_WHITE_DETECTED;
									} 
							break;
	}


//if (state==2) LcdTextf(1, 0, LcdRowToY(6), "Encontrada...");
} while (state!=2);

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}

int StraighbyDegreesToLine(int lightSensor, int speed, bool brake){
/*!< Straigh move controlled by rotations sensors and finished on line detection*/

int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
//int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors
int state = 0; // state for line transition detection, white = 1, black and previous white = 2, other 0

int lightNow = NO_LINE_DETECTED; //input from light sensor in light reflected mode


// OnFwdReg(Robot.MotorLeft, speed);
// OnFwdReg(Robot.MotorRight, speed);
OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do {
	//desviation measurement
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
    rotationsRight = MotorRotationCount(Robot.MotorRight);
	//angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	error = rotationsRight - rotationsLeft; // For Proportional part
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

	/*rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight); */
	lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);

	if (lightNow >= WHITE_DETECTED) {
		state = LINE_WHITE_DETECTED;
	} else { 
		state = NO_LINE_DETECTED; 
	  }

} while ((lightNow <= BLACK_DETECTED) && (state == LINE_WHITE_DETECTED));

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	

}


void CreateMenuKeys(int level, int (*up)(), int (*right)(), int (*down)(), int (*left)(), char  *upText,char  *rightText,char  *downText,char  *leftText){

MenuButton[level][MENU_UP].name = upText;
MenuButton[level][MENU_UP].function = up;
MenuButton[level][MENU_RIGHT].name = rightText;
MenuButton[level][MENU_RIGHT].function = right;
MenuButton[level][MENU_DOWN].name = downText;
MenuButton[level][MENU_DOWN].function = down;
MenuButton[level][MENU_LEFT].name = leftText;
MenuButton[level][MENU_LEFT].function = left;

}

void DoNothing(void){

}

void DrawMenu( int level){
	LcdClean();
	LcdTextf(1, 0, LcdRowToY(2), "UP    : %s", MenuButton[level][MENU_UP].name);
	LcdTextf(1, 0, LcdRowToY(4), "RIGHT : %s", MenuButton[level][MENU_RIGHT].name);
	LcdTextf(1, 0, LcdRowToY(6), "DOWN  : %s", MenuButton[level][MENU_DOWN].name);
	LcdTextf(1, 0, LcdRowToY(8), "LEFT  : %s", MenuButton[level][MENU_LEFT].name);
	LcdTextf(1, 0, LcdRowToY(10), "Press CENTER for NEXT Menu");
}

void MenuButtons (){
	int level = 0;
	bool exitButtonPressed = false;

	unsigned int buttonPressed = 0; 
	

	while (!exitButtonPressed){
		DrawMenu(level);
		buttonPressed = ButtonWaitForAnyPress(0);
        buttonPressed &= 0x3F;
		switch(buttonPressed) {
			case 1 :  MenuButton[level][MENU_UP].function();     //UP
					  break;
			case 2 : ++level;	  //ENTER
			        if (level >= MAXLEVEL) {
						level = 0;
					}
					break;
			case 4 : MenuButton[level][MENU_DOWN].function(); 	  //DOWN
					break;
			case 8 : MenuButton[level][MENU_RIGHT].function(); // RIGHT
			    	break;
			case 16 : MenuButton[level][MENU_LEFT].function(); 		//LEFT
					break;
			case 10 : exitButtonPressed = true;     //CENTER + Right
					break;		//ESC
			//default :
		}
		
	}
}
