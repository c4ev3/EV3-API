#include <ev3.h>
#include <ev3_robot.h>

/** Here the mechanical and design propierties of the robot must be defined
*/
ROBOT_PARAMS my_robot = {
	.Width = 114.0f, //mm
	.WheelDiameter = 62.4f, //mm
	.ColorLeftPort = IN_2,
	.ColorRightPort = IN_3,
	.ColorCenterPort = NA,  //Not used
	.ColorAuxPort = NA,	//Not used
	.LeftMotorPort	= OUT_B,
	.RightMotorPort = OUT_C,
	.ArmAMotorPort = OUT_A,
	.ArmBMotorPort = OUT_D,
	.GyroPort = IN_1,
	.GyroAuxPort = NA, //Not used 
	.Kfriction = 1.0f,
	.GearMotor = 1,
	.GearWheel = 1,
	.Direction = NORMAL
};



bool isUpButtonPressed() {
    return ButtonIsDown(BTNUP);
}
bool isExitButtonPressed() {
    return ButtonIsDown(BTNUP);
}
bool isDownButtonPressed() {
    return ButtonIsDown(BTNDOWN);
}
bool isCenterButtonPressed() {
    return ButtonIsDown(BTNCENTER);
}

int Salida1() {
	SetLightPID(2,0,0.01,3.0);
	ResetPowerCounters();
	FollowLineDegreesToLine(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 75, true, my_robot.ColorRightPort, true);
	PlayTone(TONE_B2,1000);
return 0;	
}

int Salida2() {
	SetLightPID(1.5,0.01,3.0);
	ResetPowerCounters();
	StraighbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 60, false);
	StraighbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 30, false);
	TurnGyroLeftAbs(-90,30);
	PlayTone(TONE_A2,1000);
return 0;	
}
int Salida3() {
	
	int head, angulo;
	
	angulo = TurnGyro(180, 40,45);
	head = GetPoseHead();
	PlayTone(TONE_C2,1000);
	return 0;	
}
int Salida4() {
	int head, angulo;
			
	angulo = TurnGyro(-90, 40,45);
	head = GetPoseHead();
	PlayTone(TONE_C2,1000);
return 0;	
}


int Aux1() {
	ResetEV3GyroSensor(IN_1, EV3GyroSoftwareOffset);
	return 0;	
}
int Aux2() {
return 0;	
}
int Aux3() {
return 0;	
}
int Aux4() {
return 0;	
}


int main (int argc, char **argv)  {

    unsigned long tiempo;
	LcdClean();
    InitEV3();
    SetAllSensors(EV3Gyro, EV3Color, EV3Color, NULL);
    ResetRotationCount(OUT_BC);
	LcdTextf(1, 0, LcdRowToY(2), "Robot Activate...");
	ResetEV3GyroSensor(IN_1, EV3GyroSoftwareOffset);
	LcdTextf(1, 0, LcdRowToY(2), "Gyro reseted...");

	Wait(1000);
	RobotInit(&my_robot, 1);

    CreateMenuKeys(0, Salida1, Salida2, Salida3, Salida4, "Exit 1","Exit 2","Exit 3","Exit 4");
	CreateMenuKeys(1, Aux1, Aux2, Aux3, Aux4, "Reset Gyro Sensor","Aux_2","Aux_3","Aux_4");

	MenuButtons();
	LcdClean();
	FreeEV3();
	return 0;
}
