#include "main.h"
#include <ev3.h>
#include <ev3_robot.h>

int vreloj = 10;
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

int MoveArmStallProtected(int ArmMotorPort, int speed, int angle, bool brake){
	     
	bool busy = false;
	int cS = 0;
	int mipower;
	int stallPower = 5;
	
	RotateMotorNoWaitEx(ArmMotorPort, speed, angle, 0, false, brake);
	 
  	while (true)
  		{
		Wait(MS_2); // 2ms between checks
		if (cS>=49)
			{
			mipower = abs(MotorPower(ArmMotorPort));
			if (mipower < stallPower) break;
			cS = 0;
		}
		OutputTest(ArmMotorPort, &busy);
		if (!busy) break;
		cS++;
	}
	return MotorRotationCount(my_robot.ArmAMotorPort);
}

int MoveArmTimeProtected(int ArmMotorPort, int speed, int angle, bool brake, unsigned long safetyTime){
	     
	bool busy = false;

	unsigned long elapsedTime;
	
	RotateMotorNoWaitEx(ArmMotorPort, speed, angle, 0, false, brake);
	SetTimer(1,0);
	 
  	while (true)
  		{
		Wait(MS_2); // 2ms between checks
		elapsedTime = Timer(1);
		if (elapsedTime > safetyTime) break;
		OutputTest(ArmMotorPort, &busy);
		if (!busy) break;
	}
	return MotorRotationCount(my_robot.ArmAMotorPort);
}

int SalidaBanco(){

	SetLightPID(0.75,0.01,3.0);
	ResetEV3GyroSensor(IN_1, EV3GyroSoftwareOffset);
	StraightbyGyroDegrees(CalculateTravelDegrees(470),0,60,true,true);
	// StraightbyGyroDegrees(CalculateTravelDegrees(50),0,60,true,true);
	PlayTone(TONE_F2,2000);
	StraightbyGyroDegrees(CalculateTravelDegrees(470),0,-60,true,true);
	PlayTone(TONE_B2,2000);
		return 0;	
}

int SalidaMiguel_G(){
	int armAngle;

	LcdClean();
	SetLightPID(0.75,0.01,3.0);
	StraightbyGyroDegreesWithRamps(750,0,60,10,15,true,true);
	TurnGyro(-90, 40, 30);
	StraightbyGyroDegrees(950,-90,60,true,false);
	StraighbyGyroDegreesToLine(my_robot.ColorLeftPort, -90, 25, true);
	TurnGyro(-135, 30, 30);
	StraightbyGyroDegrees(65,-135,30,true,true);
	ResetRotationCount(my_robot.ArmAMotorPort);
	TurnGyro(-135, 30, 30);
	// Prueba canasta
	armAngle=MoveArmStallProtected(my_robot.ArmAMotorPort, 40, 1000, true);
	ResetRotationCount(my_robot.ArmAMotorPort);
	armAngle=MoveArmStallProtected(my_robot.ArmAMotorPort, -40, 1300, true);
	LcdTextf(1, 0, LcdRowToY(1), "arm_fin...%d",armAngle);
	// Fin prueba canasta
	//Camino Bocchia
	StraightbyGyroDegrees(65,-135,-100,true,true);
	TurnGyro(0, 40, 30);
	StraightbyGyroDegreesWithRamps(750,0,60,10,15,true,true);
    //FollowLineDegreesToLine(CalculateTravelDegrees(800), my_robot.ColorRightPort, 50, 40, true, my_robot.ColorLeftPort, false);
	//StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(160),0,60,10,true,true);
	//mision 8
	PlayTone(TONE_B2,NOTE_WHOLE);
	Wait(5000);
	//TurnGyroRightAbs(-180, 40);
	//StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(700),-180,65,10,15,true,true);
	return 0;	
}
 
int SalidaMiguel_V(){

	SetLightPID(0.75,0.01,3.0);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(900),0,60,10,15,true,true);
	TurnGyroRightAbs(-90, 40);
    //FollowLineDegreesToLine(my_robot.ColorRightPort, -90, 50, true);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(160),0,60,10,true,true);
	//mision 8
	PlayTone(TONE_B2,2000);
	Wait(2000);
	TurnGyroRightAbs(-180, 40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(700),-180,65,10,15,true,true);
	return 0;	
}

int SalidaCarmen(){

	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1120),0,60,10,15,true,true);
	//misionneumaticos
	PlayTone(TONE_B2,2000);
	Wait(2000);
	// fin neumticos
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(440),0,60,10,15,true,true);
	//misioncintas
	PlayTone(TONE_B2,2000);
	Wait(2000);
	//fin mision cintas
	//StraighbyGyroDegreesToLine(my_robot.ColorLeftPort, 0, 30, true);
	TurnGyroRightAbs(-90, 40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(217),-90,40,10,15,true,true);
	//mision maquina remo
	PlayTone(TONE_B2,2000);
	Wait(2000);
	// fin maquina remo
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(400),-90,60,10,40,true,true);
	TurnGyroRightAbs(-180, 40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1000),-180,60,10,10,true,true);

return 0;
}

int Salida1() {
	SetLightPID(0.75,0.01,3.0);/* 2,0,3 para 40 speed*/
	/*SetLightPID(1.5,0.01,3.0); para 40 */
	ResetPowerCounters();

	//FollowLineDegrees(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 40, true, true);
	FollowLineDegreesToLine(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 75, true, my_robot.ColorRightPort, true);


	PlayTone(TONE_B2,2000);
return 0;	
}

int Salida2() {

	SetLightPID(0.25,0.01,3.0);/* 2,0,3 para 40 speed*/
	SetLightPID(1.5,0.01,3.0);
	ResetPowerCounters();

	FollowLineDegreesToLine(CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, false, my_robot.ColorLeftPort, true);

	//StraighLAGDegrees(CalculateTravelDegrees(600), my_robot.ColorLeftPort, ReadEV3ColorSensorReflectedLight(my_robot.ColorLeftPort),0, 60, true, true);
	/*
		int head, angulo;
			
	angulo = TurnGyro(-90, 40,45);
	head = GetPoseHead();

	
*/

	/*int distancia;
		LcdClean();
		StraighbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 60, false);
		StraighbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 30, false);
 		TurnGyroLeftAbs(-90,30);
		 distancia = StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(500),-90,70,30,50,true, true);
		vreloj ++;
	LcdTextf(1, 0, LcdRowToY(2), "Velocidad: %d",vreloj);*/
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
			
	angulo = TurnGyro(90, 40,45);
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
    /**
     * Initialize EV3Gyro sensor connected at port 1
     */
     //unsigned long tiempo;
	LcdClean();
    InitEV3();
    SetAllSensors(EV3Gyro, EV3Color, EV3Color, NULL);
    ResetRotationCount(OUT_BC);
	LcdTextf(1, 0, LcdRowToY(2), "Robot Activando...");
	ResetEV3GyroSensor(IN_1, EV3GyroSoftwareOffset);
	LcdTextf(1, 0, LcdRowToY(2), "Gyro reseteado...");

	Wait(2000);
	RobotInit(&my_robot, 1);

    CreateMenuKeys(0, SalidaMiguel_V, SalidaCarmen, SalidaMiguel_G, SalidaBanco, "Salida M Vera","Salida Carmen","Salida M Gomez","Salida Banco");
	CreateMenuKeys(1, Aux1, Aux2, Aux3, Aux4, "Reset Sensor Giro","Auxiliar_2","Auxiliar_3","Auxiliar_4");

	MenuButtons();
	LcdClean();
	FreeEV3();
	return 0;
}
