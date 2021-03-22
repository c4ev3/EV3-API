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
	.Direction = NORMAL,
	.minColorLeft = 37,
	.maxColorLeft = 351,
	.minColorRight = 36,
	.maxColorRight = 352
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

int TurnToBlack(int direction, int lightSensor, int threshold, bool brake){ //En desarrollo
	
	
	int angleNow;
	int lightNow;
	int spin;

	if (direction == TO_RIGHT) spin = SPIN_RIGHT;
	else spin = SPIN_LEFT;
	lightNow = ReadLight(lightSensor);
	if (lightNow < threshold) spin *= -1;
	OutputTimeSync(OUT_BC, MIN_SPEED_SPIN, spin, 0);

	while (true){

		//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
		lightNow = ReadLight(lightSensor);	
		if (lightNow < threshold) {
			if (brake) Off(OUT_BC);
			break;
		}

	}
	
	angleNow = ReadEV3GyroSensorAngle(IN_1, EV3GyroNormalAngle);
	NewHeadPose();
	return(angleNow);
}

int MyMoveArmStallProtected(int ArmMotorPort, int speed, int angle, bool brake){
	     
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
	return MotorRotationCount(my_robot.ArmAMotorPort);
}

int MyMoveArmTimeProtected(int ArmMotorPort, int speed, int angle, bool brake, unsigned long safetyTime){
	     
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
	Float(ArmMotorPort);
	return MotorRotationCount(my_robot.ArmAMotorPort);
}

int SalidaBanco(){

	SetLightPID(0.75,0.01,3.0);
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	StraightbyGyroDegrees(CalculateTravelDegrees(470),0,60,true,true);
	// StraightbyGyroDegrees(CalculateTravelDegrees(50),0,60,true,true);
	StraightbyGyroDegrees(CalculateTravelDegrees(470),0,-60,true,true);
	//PlayTone(TONE_B2,2000);
		return 0;	
} 

int SalidaMiguel_G(){
	int armAngle;
	LcdClean();
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	ResetCounterMotor(my_robot.ArmAMotorPort);
	SetLightPID(0.75,0.0001,1.0);
	SetLightPID(0.5,0.001,2.0);
	ResetRotationCount(my_robot.ArmAMotorPort);
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 20, 40, 3000, true);
	StraightbyGyroDegreesWithRamps(750,0,60,10,15,true,true); //avanzar
	TurnGyro(-90, 40, 30); //giro -90
	StraightbyGyroDegrees(360,-90,60,true,false); //avanzar 950, head -90 vel 60
//	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, -90, 30, 15, true); //Avanzar hasta linea blanca
    //TurnToBlack(TO_RIGHT, my_robot.ColorRightPort, 50, true);
	ResetPowerCounters();
	FollowLineDegrees(750, my_robot.ColorRightPort, 50, 40, true, true);
	//ResetPowerCounters();
	TurnGyro(-90, 40, 30); //giro -90
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, -90, 20, 10, true);
	//int StraightbyGyroDegreesToUmbral(int lightSensor, int angle, int speed, int umbral, bool brake);
	//FollowLineDegreesToUmbral(CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, my_robot.ColorLeftPort, 85, true, true, true);
	
	//FollowLineDegreesToLine(CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, false, my_robot.ColorLeftPort, true);
	//TurnGyro(-65, 30, 30); //prueba conjunta
	/*armAngle=MoveArmStallProtected(my_robot.ArmAMotorPort, 100, 500, true); //subir brazo izq
	ResetRotationCount(my_robot.ArmAMotorPort);
	armAngle=MoveArmStallProtected(my_robot.ArmAMotorPort, -100, 500, false); 
	ResetRotationCount(my_robot.ArmAMotorPort); */
	TurnGyro(-135, 30, 30); // girar hasta canasta
	StraightbyGyroDegrees(120,-135,30,true,true); //avanzar hasta canasta
	ResetRotationCount(my_robot.ArmAMotorPort);
	TurnGyro(-135, 30, 30); //ajustar angulo
	// Prueba canasta
	Wait(3000);

	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 60, 1100, 4000,true); //subir brazo izq
	ResetRotationCount(my_robot.ArmAMotorPort);
	armAngle=MoveArmStallProtected(my_robot.ArmAMotorPort, -60, false, 1100); 
	ResetRotationCount(my_robot.ArmAMotorPort);
	//bajar brazo der
	Float(my_robot.ArmAMotorPort);
	ResetRotationCount(my_robot.ArmAMotorPort);

	LcdTextf(1, 0, LcdRowToY(1), "arm_fin...%d",armAngle);
	// Fin prueba canasta
	//Camino Bocchia
	ResetPowerCounters();
	StraightbyGyroDegrees(350,-135,-65,true,true);  //retroceder
	TurnGyro(0, 40, 30); //girar a 0 hacia otras pruebas
	StraightbyGyroDegreesWithRamps(950,0,60,10,15,true,true); //avanzar 
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
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	ResetCounterMotor(my_robot.ArmAMotorPort);
	SetLightPID(0.1,0.1,3.0);
	
	//Motor bomba en continuo desde el principio

//	StraighLAGDegrees(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 0, 40, EDGELEFT, true);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(850),0,75,10,40,true,false); //900
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(1050),0,40,10,false,true); //Empujar cuanta pasos
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(85),0,-40,-10,-10,true,true); //Retroceder 10 cm
	//TurnGyroRightAbs(-90, 40);
	TurnGyro(-90,40,30);
	OnFwdEx(my_robot.ArmBMotorPort,100,RESET_NONE); // inicio bombeo aire
    StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(300),-90,75,5,60,true,false);
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, -90,60,80,false);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(260), -90, 75, 5, true, true); //llegada a Boccia
	// Aqui empieza el rocknroll
	//OnFwdEx(my_robot.ArmBMotorPort,150,RESET_NONE);
	MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 525, 2000, true); //Calculates ratio por 63 grados de apertura del switch neumatico
	ResetCounterMotor(my_robot.ArmAMotorPort);
	Wait(1000); // subir teleferico y tirar bolas Boccia
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(360),-90,-60,-5,true,true); //Retrocedemos hasta el arco
    MoveArmTimeProtected(my_robot.ArmAMotorPort, -100, 525, 2000, true);
    Wait (10000); // 10 segundos antes de parar
	Off(my_robot.ArmBMotorPort);
	return 0;

}

int SalidaCarmen(){

	LcdClean();
	SetStraightPID(2.0f, 0.0f, 0.0f);
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	ResetCounterMotor(my_robot.ArmAMotorPort);
	ResetCounterMotor(my_robot.ArmBMotorPort);
	SetLightPID(0.1,0.1,3.0);
	 
  




	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1070),0,75,10,0,true,true); //1080
	int brazo = MoveArmTimeProtected(my_robot.ArmBMotorPort, -100, 200, 2000, true);  //180
	LcdTextf(1, 0, LcdRowToY(1), "arm_fin...%d",brazo);
	//Wait(2000);
	//StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(460),0,75,10,0,true,true);// 460 hasta prueba ponemos 300 y luego hasta linea negra
	StraightbyGyroDegreesWithAccel(CalculateTravelDegrees(300),0,75,10,true,false);
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort,0,40,10,false);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(20),0,40,5,true,true);
	OnFwdEx(my_robot.ArmBMotorPort,-100,RESET_NONE);
	Wait(4000);
	Off(my_robot.ArmBMotorPort); //fin rueda
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(150),0,-75,-10,0,true,true);
	//si eso giro del brazo a para coger corazon
	TurnGyro(-30,40,30);
	brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 200, 2000, true); 
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(240),-30,75,10,0,true,true);
	brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, -100, 200, 2000, true); 



	
	


	//StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1000),0,-75,-10,0,true,true);
	/*SetStraightPID(5.0f, 0.0f, 0.0f);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(320),0,60,10,true,true);
	SetStraightPID(2.0f, 0.0f, 0.0f);
	//misionneumaticos
	PlayTone(TONE_B2,2000);
	Wait(2000);
	// fin neumticos
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(440),0,60,10,15,true,true);
	//misioncintas
	PlayTone(TONE_B2,2000);
	Wait(2000);
	//fin mision cintas
	//StraightbyGyroDegreesToLine(my_robot.ColorLeftPort, 0, 30, true);
	TurnGyroRightAbs(-90, 40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(217),-90,40,10,15,true,true);
	//mision maquina remo
	PlayTone(TONE_B2,2000);
	Wait(2000);
	// fin maquina remo
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(400),-90,60,10,40,true,true);
	TurnGyroRightAbs(-180, 40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1000),-180,60,10,10,true,true);
*/
return 0;
}

void  CalibrateLightOriginal() {
	RawReflect lightRawLeft;
	RawReflect lightRawRight;
	int minLightLeft;
	int minLightRight;
	int maxLightRight;
	int maxLightLeft;
	int i = 0;

	FILE *myFile;

	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawRight);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawRight);
	minLightLeft = lightRawLeft.reflection;
	maxLightLeft = lightRawLeft.reflection;
	minLightRight = lightRawRight.reflection;
	maxLightRight = lightRawRight.reflection;


	OnFwdSync(OUT_BC,10);
	SetTimerMS(1,0);
	while (TimerMS(1) < 2000){
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorRightPort, &lightRawRight);
	if (lightRawLeft.reflection < minLightLeft) minLightLeft = lightRawLeft.reflection;
	if (lightRawRight.reflection < minLightRight) minLightRight = lightRawRight.reflection;
	if (lightRawLeft.reflection > maxLightLeft) maxLightLeft = lightRawLeft.reflection;
	if (lightRawRight.reflection > maxLightLeft) maxLightRight = lightRawRight.reflection;
	i++;
	}

	Off(OUT_BC);
	LcdTextf(1, 0, LcdRowToY(2), "Iteraciones %d", i);
	LcdTextf(1, 0, LcdRowToY(3), "en %d", TimerMS(1) );
	LcdTextf(1, 0, LcdRowToY(4), "Min izq Raw... %d", minLightLeft);
	LcdTextf(1, 0, LcdRowToY(5), "Max izq Raw... %d", maxLightLeft);
	LcdTextf(1, 0, LcdRowToY(6), "Min der Raw... %d", minLightRight);
	LcdTextf(1, 0, LcdRowToY(7), "Max der Raw... %d", maxLightRight);
	myFile = fopen("/home/root/lms2012/prjs/rt21/lightcal.txt", "w");
	if (myFile == NULL) LcdTextf(1, 0, LcdRowToY(8), "Fichero error");
	Wait(2000);
	fprintf(myFile,"%d",(int)minLightLeft);
	fprintf(myFile,"%d",(int)maxLightLeft);
	fprintf(myFile,"%d",minLightRight);
	fprintf(myFile,"%d",maxLightRight);
	fclose(myFile);

}
int Salida1() {
	int lightNow;
	int lightNow2;
	RawReflect lightRaw;
	RawReflect lightRawd;
	int pcLeft;
	int pcRight;

unsigned long elapsedTime;
unsigned long elapsedTime2;
unsigned long elapsedTime3;
    LcdClean();
	CalibrateLight();
	
	SetTimerMS(1,0);
	while (true)
	{

	ClearTimerMS(1);
	lightNow = ReadEV3ColorSensorReflectedLight(my_robot.ColorLeftPort);
	lightNow2 = ReadEV3ColorSensorReflectedLight(my_robot.ColorRightPort);
	elapsedTime = TimerMS(1);
	ClearTimerMS(1);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRaw);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorRightPort, &lightRawd);
	elapsedTime2 = TimerMS(1);
	ClearTimerMS(1);
	pcLeft = ReadLight(my_robot.ColorLeftPort);
	pcRight = ReadLight(my_robot.ColorRightPort);
	elapsedTime3 = TimerMS(1);
	LcdClean(); 
	LcdTextf(1, 0, LcdRowToY(1), "Sensor izd... %d", lightNow);
	LcdTextf(1, 0, LcdRowToY(2), "Ref izq Raw... %d", lightRaw.reflection);
	//LcdTextf(1, 0, LcdRowToY(3), "Back izq Raw... %d", lightRaw.background);
	LcdTextf(1, 0, LcdRowToY(3), " Pct izq.. %d", pcLeft);

	LcdTextf(1, 0, LcdRowToY(4), "Sensor der... %d", lightNow2);
	LcdTextf(1, 0, LcdRowToY(5), "Ref der Raw... %d", lightRawd.reflection);
	//LcdTextf(1, 0, LcdRowToY(6), "Back der Raw... %d", lightRawd.background);
	LcdTextf(1, 0, LcdRowToY(6), "Pct der... %d", pcRight);
	
	LcdTextf(1, 0, LcdRowToY(7), "TIME std... %lu", elapsedTime);
	LcdTextf(1, 0, LcdRowToY(8), "tIME raw... %lu", elapsedTime2);
	LcdTextf(1, 0, LcdRowToY(9), "tIME pc... %lu", elapsedTime3);
	Wait(1000);
	}
	//SetLightPID(0.75,0.01,3.0);
	/* 2,0,3 para 40 speed*/
	/*SetLightPID(1.5,0.01,3.0); para 40 */
	//7ResetPowerCounters();

	//FollowLineDegrees(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 40, true, true);
	//FollowLineDegreesToLine(CalculateTravelDegrees(600), my_robot.ColorLeftPort, 50, 75, true, my_robot.ColorRightPort, true);


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
		StraightbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 60, false);
		StraightbyGyroDegreesToLine(my_robot.ColorRightPort, 0, 30, false);
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
	LcdClean();
	CalibrateLight();
return 0;	
}
int Aux3() {
	ResetPowerCounters();
	SetLightPID(2.0,0.01,1.0);
	FollowLineDegrees( CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, true, true);
return 0;	
}
int Aux4() {
	ResetPowerCounters();
	SetLightPID(1.0,0.01,3.0);
	FollowLineDegrees( CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, false, true);

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
	CreateMenuKeys(1, Aux1, Aux2, Aux3, Aux4, "Reset Sensor Giro","Calibrate Color","Auxiliar_3","Auxiliar_4");

	MenuButtons();
	LcdClean();
	FreeEV3();
	return 0;
}
