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
 * \date 2021-03-20
 * \version 1.1.0
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
	int minColorLeft;
	int maxColorLeft;
	int minColorRight;
	int maxColorRight;
	float lightLeftScale;
	float lightRightScale;

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

char nomFileCalibration[] = "/home/root/lms2012/prjs/lightcalibration.cal";


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

void PowerFloat (){
SetOutputEx(Robot.MotorDual, OUT_FLOAT, 0);
}

int TurnGyro(int angle, int speed, int threshold){
	
	int direction = 1; /* default case turn to the right */
	int distance;
	int angleNow;
	
	while (true){

		angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3_GYRO_SENSOR_ANGLE_MODE);
		distance = angle - angleNow;
		
		if (distance > 0) direction = 1;
		else direction = -1;
		
		if (distance == 0) {
			Off(Robot.MotorDual);
			break;
		}
		else if (abs(distance) <= threshold) {
			OutputTimeSync(Robot.MotorDual, MIN_SPEED_SPIN, SPIN_RIGHT*direction, 0);
		}
		else  {
			OutputTimeSync(Robot.MotorDual, speed, SPIN_RIGHT*direction, 0);
		}
	}
	NewHeadPose();
	return(angleNow);
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
	SetPoseHead(0);
	return (ReadEV3GyroSensorAngle(port, EV3GyroNormalAngle));	
}
 
void ResetPowerCounters(){ 
	
	ResetCount(Robot.MotorDual,RESET_ALL);
}

void ResetCounterMotor(int motorPort){

	ResetCount(motorPort,RESET_ALL);
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
	if (!ReadCalibration()){
	Robot.minColorLeft =  params->minColorLeft;
	Robot.maxColorLeft = params->maxColorLeft;
	Robot.minColorRight = params->minColorRight;
	Robot.maxColorRight = params->maxColorLeft;
	}
	
	LcdTextf(1, 0, LcdRowToY(4), "Min izq ... %d", Robot.minColorLeft);
	LcdTextf(1, 0, LcdRowToY(5), "Max izq ... %d", Robot.maxColorLeft);
	LcdTextf(1, 0, LcdRowToY(6), "Min der ... %d", Robot.minColorRight);
	LcdTextf(1, 0, LcdRowToY(7), "Max der ... %d", Robot.maxColorRight);
	
	Robot.lightLeftScale = (Robot.maxColorLeft - Robot.minColorLeft) / 100.0f;
	Robot.lightRightScale = (Robot.maxColorRight - Robot.minColorRight) / 100.0f;

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
float kp = StraightPid.Kp;

if  (speedTravel < 0) kp = -kp;

kAccel = ((speedTravel - speedInit) / RAMP_UP); // eliminado (int)
kDaccel = ((speedTravel - speedEnd) / RAMP_DOWN);

if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
initTravel = abs((int)((rotationsLeft + rotationsRight) / 2)); 
traveled = initTravel;


//RotateMotorRampNoWait()
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
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
	traveled = abs((int)((rotationsLeft + rotationsRight) / 2)); 	

}while (traveled < distDegree);

if (brake) Off(Robot.MotorDual);

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
float kAccel;
int ramp; //Degrees traveled in ramp state
int relativeTraveled;
float kp = StraightPid.Kp;

if  (speedTravel < 0) kp = -kp;

kAccel = ((speedTravel - speedInit) / RAMP_UP);

if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
ramp = abs((int)((rotationsLeft + rotationsRight) / 2)); 
traveled = ramp;
//RotateMotorRampNoWait()
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
	oldErrorD = error;
	
	relativeTraveled = traveled - ramp;
	if (relativeTraveled < RAMP_UP) {
		speed = speedInit + (int)(relativeTraveled*kAccel);
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
	traveled = abs((int)((rotationsLeft + rotationsRight) / 2)); 	

}while (traveled <= distDegree);

if (brake) Off(Robot.MotorDual);

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

int distDegreeAbs = abs(distDegree);

float kp = StraightPid.Kp;



if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);

if  (speed < 0) kp = -kp;

if (distDegree < 0) {
	speed = -speed;
	kp = -kp;
}

do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
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
	traveled = abs((int)((rotationsLeft + rotationsRight) / 2)); 	

}while (traveled < distDegreeAbs);

if (brake) Off(Robot.MotorDual);
// To DO Use ramp function
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
return (int)((rotationsLeft + rotationsRight) / 2); 	

}

int StraightbyGyroDegreesWithBrake(int distDegree, int angle, int speedTravel, int speedEnd, bool resetCounters, bool brake){

int traveled ; //= 0;
int rotationsLeft ;//= MotorRotationCount(Robot.MotorLeft);
int rotationsRight ;//= MotorRotationCount(Robot.MotorRight);
int angleNow ; //= ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part
int speed = speedTravel;
int uOut = 0; // control for motors

float kDaccel;
int initTravel; //Degrees traveled in accelaration ramp state

int remainTravel;
float kp = StraightPid.Kp;


if  (speedTravel < 0) kp = -kp;

kDaccel = (int)((speedTravel - speedEnd) / RAMP_DOWN);

if (resetCounters) ResetCount(Robot.MotorDual,RESET_ALL);
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
initTravel = abs((int)((rotationsLeft + rotationsRight) / 2)); 
traveled = initTravel;


//RotateMotorRampNoWait()
do{
	//desviation measurement
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	error = angle - angleNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  kp) + (errorI * StraightPid.Ki) + (errorD * StraightPid.Kd ));
	oldErrorD = error;

	remainTravel = distDegree - traveled;

    if (remainTravel <= RAMP_DOWN) speed = speedEnd + (int)(remainTravel*kDaccel);
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
	traveled = abs((int)((rotationsLeft + rotationsRight) / 2)); 	

}while (traveled < distDegree);

if (brake) {
	Off(Robot.MotorDual);
}
// To DO Use ramp function
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}

int FollowLineDegrees(int distDegree, int lightSensor, int light, int speed, int edgeSide, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors


OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(lightSensor);

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
	//if (inOutSide) uOut = -uOut; // In case of follow south/east border line then reverse
	uOut *= edgeSide;

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


int StraightLAGDegrees(int distDegree, int lightSensor, int light, int angle, int speed, int edgeLine, bool brake){
/*!< Based on LAGS Line Assisted Gyro Steering idea from Brickwolves Waring FLL Team*/

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int errorLight;
int errorGyro;
int uOut = 0; // control for motors
int angleObjetive = angle;

OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	lightNow = ReadLight(lightSensor);
	angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
	
	errorLight = light - lightNow;
	errorLight *= edgeLine;         // For Proportional part due to light
	angleObjetive = angle + (int)(errorLight * LightPid.Kp);
	errorGyro = angleObjetive - angleNow;//
	uOut = (int)(errorGyro *  StraightPid.Kp);	
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
return traveled;	
}

int StraightbyGyroDegreesToLine(int lightSensor, int angle, int speed, bool brake){
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
int traveled;


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
	traveled = (int)((rotationsLeft + rotationsRight) / 2);

//	lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(lightSensor);

	switch (state){

		case LINE_WHITE_DETECTED: if (lightNow <= BLACK_DETECTED) state = LINE_DETECTED;
								 else if ((traveled - stripBorder) > degreesStrip) state = NO_LINE_DETECTED;
							break;
		
		case NO_LINE_DETECTED: if (lightNow >= WHITE_DETECTED) {
									stripBorder = traveled;
									state = LINE_WHITE_DETECTED;
									} 
							break;
	}
} while (state!=LINE_DETECTED);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
// To DO Use ramp function
return traveled;		
}

int StraightbyGyroDegreesToUmbral(int lightSensor, int angle, int speed, int umbral, bool brake){
/*!< Straigh move controlled by gyro and finished on line detection*/

int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int angleNow = ReadEV3GyroSensorAngle(Robot.Gyro, EV3GyroNormalAngle);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors
int dunmmy;

int lightNow ; //input from light sensor in light reflected mode

int stripBorder=0; //distance from white detected
int traveled;
float kp = StraightPid.Kp;

if  (speed < 0) kp = -kp;

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
	traveled = (int)((rotationsLeft + rotationsRight) / 2);

	//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(lightSensor);


	if ((umbral >= 50) && (lightNow >= umbral)) break;
	if ((umbral < 50) && (lightNow < umbral)) break;

} while (true);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
// To DO Use ramp function
return traveled;		
}

bool FindUmbral (int lightSensor, int angle, int umbral){
	int lightNow;
	
	OutputStepSyncEx(Robot.MotorDual, 8, SPIN_RIGHT, angle, true, OWNER_NONE);
		do
		{
			lightNow = ReadLight(lightSensor);
			if (((umbral >= 50) && (lightNow >= umbral))||((umbral < 50) && (lightNow < umbral))){
				return OutputStop(Robot.MotorDual, true);
			} 
		} while (ReadEV3GyroSensorAngle < angle);

	OutputStepSyncEx(Robot.MotorDual, 8, SPIN_LEFT, -angle, true, OWNER_NONE);
		do
		{
			lightNow = ReadLight(lightSensor);
			if (((umbral >= 50) && (lightNow >= umbral))||((umbral < 50) && (lightNow < umbral))){
				return OutputStop(Robot.MotorDual, true);
			} 
		} while (ReadEV3GyroSensorAngle > angle);
	
	return false;

}

int RevToUmbral(int lightSensor, int speed, int umbral, bool brake){
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);

int lightNow ; //input from light sensor in light reflected mode

int traveled;


OnRevEx(Robot.MotorDual, speed, 0);

	do {
		rotationsLeft = MotorRotationCount(Robot.MotorLeft);
		rotationsRight = MotorRotationCount(Robot.MotorRight);
		traveled = (int)((rotationsLeft + rotationsRight) / 2);
		lightNow = ReadLight(lightSensor);


		if ((umbral >= 50) && (lightNow >= umbral)) break;
		if ((umbral < 50) && (lightNow < umbral)) break;
	
	} while (true);

if (brake) Off(Robot.MotorDual);
	 	
rotationsLeft = MotorRotationCount(Robot.MotorLeft);
rotationsRight = MotorRotationCount(Robot.MotorRight);
return (int)((rotationsLeft + rotationsRight) / 2);
	
}



/* to line */
int FollowLineDegreesToLine(int distDegree, int lightSensor, int light, int speed, bool inOutSide, int lineSensor, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
//int lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
int lightNow = ReadLight(lightSensor);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors

int state = NO_LINE_DETECTED; // state for line transition detection, white = 1, black and previous white = 2, other 0
int lightlineNow ; //input from light sensor in light reflected mode
int stripBorder=0; //distance from white detected


OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(lightSensor);
	
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
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
    traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

	//lightlineNow = ReadEV3ColorSensorReflectedLight(lineSensor);
    lightlineNow = ReadLight(lineSensor);
  
	switch (state){

		case LINE_WHITE_DETECTED: if (lightlineNow <= BLACK_DETECTED) state = LINE_DETECTED;
								  else if ((traveled-stripBorder) > degreesStrip) state = NO_LINE_DETECTED;
							break;
		
		case NO_LINE_DETECTED: if (lightlineNow >= WHITE_DETECTED) {
									stripBorder = traveled;
									state = LINE_WHITE_DETECTED;
									} 
							break;
	}

}while (state!=LINE_DETECTED);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
// To DO Use ramp function
return traveled;	
// To DO Use ramp or 
}

int FollowLineToUmbral(int followLightSensor, int light, int speed, int stopLightSensor, int umbral, int edgeSide, bool resetCounters, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
//int lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
int lightNow = ReadLight(followLightSensor);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors

int state = NO_LINE_DETECTED; // state for line transition detection, white = 1, black and previous white = 2, other 0
int lightlineNow ; //input from light sensor in light reflected mode
int stripBorder=0; //distance from white detected


OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(followLightSensor);
	
	error = light - lightNow; // For Proportional part
	errorD = error - oldErrorD; // For Darivative part
	errorI = errorI + error; //For Integral part
	//PID control
	uOut = (int)((error *  LightPid.Kp) + (errorI * LightPid.Ki) + (errorD * LightPid.Kd ));
	oldErrorD = error;
		// To DO : Check limits on DI
	//if (inOutSide) uOut = -uOut; // In case of follow south/east border line then reverse
    uOut *= edgeSide;
	OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
    traveled = (int)((rotationsLeft + rotationsRight) / 2); 	

	//lightlineNow = ReadEV3ColorSensorReflectedLight(lineSensor);
    lightlineNow = ReadLight(stopLightSensor);

	if ((umbral >= 50) && (lightlineNow >= umbral)) break;
	if ((umbral < 50) && (lightlineNow < umbral)) break;

}while (true);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return (int)((rotationsLeft + rotationsRight) / 2); 	
}
// To DO Use ramp function
return traveled;	
// To DO Use ramp or 
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
	LcdTextf(1,0,LcdRowToY(12), "X: %d, Y: %d, H:%d",GetPoseX(),GetPoseY(),GetPoseHead());
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

bool ReadCalibration() {
	FILE *myFile;

myFile = fopen(nomFileCalibration, "rb");
	if (myFile != NULL){
		fread(&Robot.minColorLeft, sizeof(int), 1, myFile);
		fread(&Robot.maxColorLeft, sizeof(int), 1, myFile);
		fread(&Robot.minColorRight, sizeof(int), 1, myFile);
		fread(&Robot.maxColorRight, sizeof(int), 1, myFile);
		fclose(myFile);
		return(true);
	}
	return (false);	
}

void  CalibrateLight() {
	RawReflect lightRawLeft;
	RawReflect lightRawRight;

	int i = 0;
	int minLightLeft;
	int maxLightLeft;
	int minLightRight;
	int maxLightRight;
	FILE *myFile;

	ReadEV3ColorSensorRawReflectedLight(Robot.ColorLeft, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(Robot.ColorRight, &lightRawRight);
	/*ReadEV3ColorSensorRawReflectedLight(Robot.ColorLeft, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(Robot.ColorRight, &lightRawRight);*/
	
	/* minLightLeft = lightRawLeft.reflection;
	maxLightLeft = lightRawLeft.reflection;
	minLightRight = lightRawRight.reflection;
	maxLightRight = lightRawRight.reflection;*/
	
	minLightLeft = lightRawLeft.reflection;
	maxLightLeft = lightRawLeft.reflection;
	minLightRight = lightRawRight.reflection;
	maxLightRight = lightRawRight.reflection;


	OnFwdSync(Robot.MotorDual,8);
	SetTimerMS(1,0);
	while (TimerMS(1) < 3000){
	ReadEV3ColorSensorRawReflectedLight(Robot.ColorLeft, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(Robot.ColorRight, &lightRawRight);
	if (lightRawLeft.reflection < minLightLeft) minLightLeft = lightRawLeft.reflection;
	if (lightRawRight.reflection < minLightRight) minLightRight = lightRawRight.reflection;
	if (lightRawLeft.reflection > maxLightLeft) maxLightLeft = lightRawLeft.reflection;
	if (lightRawRight.reflection > maxLightLeft) maxLightRight = lightRawRight.reflection;
	i++;
	}



	Off(Robot.MotorDual);
	Robot.minColorLeft = minLightLeft;
	Robot.minColorRight = minLightRight;
	Robot.maxColorLeft = maxLightLeft;
	Robot.maxColorRight = maxLightRight;
	Robot.lightLeftScale = (maxLightLeft - minLightLeft) / 100.0f;
	Robot.lightRightScale = (maxLightRight - minLightRight) / 100.0f;

	LcdTextf(1, 0, LcdRowToY(2), "Iteraciones %d in %d", i, TimerMS(1));
	LcdTextf(1, 0, LcdRowToY(3), "Raw   min    max");
	LcdTextf(1, 0, LcdRowToY(4), "Left %d - %d", minLightLeft, maxLightLeft);
	LcdTextf(1, 0, LcdRowToY(5), "Right... %d -%d", minLightRight, maxLightRight);
	LcdTextf(1, 0, LcdRowToY(6), "Scale Left %f", Robot.lightLeftScale );
	LcdTextf(1, 0, LcdRowToY(7), "Scale Right... %f", Robot.lightRightScale );
	
	myFile = fopen(nomFileCalibration, "wb");
	if (myFile != NULL){
		fwrite(&minLightLeft, sizeof(int), 1, myFile);
		fwrite(&maxLightLeft, sizeof(int), 1, myFile);
		fwrite(&minLightRight, sizeof(int), 1, myFile);
		fwrite(&maxLightRight, sizeof(int), 1, myFile);
		fclose(myFile);
	LcdTextf(1, 0, LcdRowToY(8), "Saved!");	
	}
	Wait(2000);
}

int ReadLight(int colorPort){
RawReflect lightRaw;
float lightScale;
int minLight;


		if (colorPort == Robot.ColorLeft){
			minLight = Robot.minColorLeft;
			lightScale = Robot.lightLeftScale;
		}
		else {
			minLight = Robot.minColorRight;
			lightScale = Robot.lightRightScale;
		}
		ReadEV3ColorSensorRawReflectedLight(colorPort, &lightRaw);
		return (int)((lightRaw.reflection - minLight) / lightScale);
}

int MoveArmStallProtected(int ArmMotorPort, int speed, int angle, bool brake){
	     
	bool busy = false;
	int mipower;
	int stallPower = 8;
	
	RotateMotorNoWaitEx(ArmMotorPort, speed, angle, 0, false, brake);
	Wait(MS_100); //Wait 100mS to motor start

  	while (true)
  		{
		Wait(MS_2); // 2ms between checks
		mipower = abs(MotorPower(ArmMotorPort));
		if (mipower < stallPower) break;
		OutputTest(ArmMotorPort, &busy);
		if (!busy) break;
	}
	if (busy) Float(ArmMotorPort);
	return MotorRotationCount(ArmMotorPort);
}

int MoveArmTimeProtected(int ArmMotorPort, int speed, int angle, unsigned long safetyTime, bool brake){
	     
	bool busy = false;

	unsigned long elapsedTime;
	
	RotateMotorNoWaitEx(ArmMotorPort, speed, angle, 0, false, brake);
	SetTimerMS(1,0);
	 
  	while (true)
  		{
		Wait(MS_2); // 2ms between checks
		elapsedTime = TimerMS(1);
		if (elapsedTime > safetyTime) break;
		OutputTest(ArmMotorPort, &busy);
		if (!busy) break;
	}
	if (busy) Float(ArmMotorPort);
	return MotorRotationCount(ArmMotorPort);
}

int FollowLineDegreesdevel(int distDegree, int lightSensor, int light, int speed, bool inOutSide, bool brake){

int traveled = 0;
int rotationsLeft = MotorRotationCount(Robot.MotorLeft);
int rotationsRight = MotorRotationCount(Robot.MotorRight);
int lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
int error;
int errorD = 0; // error compensation for derivative part
int oldErrorD= 0;
int errorI = 0; //accumalated errors for integral part

int uOut = 0; // control for motors
int i = 0;
unsigned long currentime, ellapsedtime, previoustime;
SetTimerMS(1,0);
previoustime = 0;
OutputTimeSync(Robot.MotorDual, speed, uOut, 0);
do{
	//desviation measurement
	//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
	lightNow = ReadLight(lightSensor);
	currentime = TimerMS(1);
	ellapsedtime = currentime - previoustime;

	error = light - lightNow; // For Proportional part
	errorD = (error - oldErrorD)/ellapsedtime; // For Darivative part
	errorI += error*ellapsedtime; //For Integral part
	//PID control
	uOut = (int)((error *  LightPid.Kp) + (errorI * LightPid.Ki) + (errorD * LightPid.Kd ));
	oldErrorD = error;
	previoustime = currentime;
	if (uOut > 50) uOut = 50;
	if (uOut < -50) uOut = -50;
	
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
	//Wait(20);   
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	traveled = (int)((rotationsLeft + rotationsRight) / 2); 	
	i++;
}while (traveled <= distDegree);

if (brake) {
	Off(Robot.MotorDual);
	rotationsLeft = MotorRotationCount(Robot.MotorLeft);
	rotationsRight = MotorRotationCount(Robot.MotorRight);
	return i;
	//return (int)((rotationsLeft + rotationsRight) / 2); 	

}
// To DO Use ramp function
return i;	 	

}