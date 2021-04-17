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
/**
Some useful notes:
Big Cremallera max 1000grados --> 27studs --->216mmm 
37grados/stud 

*/
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

void waitButton() {
	while (!isCenterButtonPressed()){}

}

int TurnToWhite(int direction, int lightSensor, bool brake){ //En desarrollo
	
	
	int angleNow;
	int lightNow;
	int spin;
	int white = 80;
	int black = 10;
	//bool blackdetected = false;

	if (direction == TO_RIGHT) spin = SPIN_RIGHT;
	else spin = SPIN_LEFT;
	lightNow = ReadLight(lightSensor);
	if (lightNow <= black) {
		spin *= -1;
		//blackdetected = true; //si black retrocedemos hasta blanco
	}
	OutputTimeSync(OUT_BC, MIN_SPEED_SPIN, spin, 0);

	while (true){

		//lightNow = ReadEV3ColorSensorReflectedLight(lightSensor);
		lightNow = ReadLight(lightSensor);	
		if (lightNow <= black) {
		spin *= -1;
		//blackdetected = true; //si black retrocedemos hasta blanco
		}
		else if ((lightNow >= white) ) break;

	}
	if (brake) Off(OUT_BC);
	angleNow = ReadEV3GyroSensorAngle(IN_1, EV3GyroNormalAngle);
	NewHeadPose();
	return(angleNow);
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
int SalidaColumpio(){

	SetLightPID(0.75,0.01,3.0);
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(440),0,75,10,10,true,true);
	Wait(1000);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(590),0,-75,-10,-20,true,true);
	TurnGyro(34,40,40);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(100),34,-75,-10,-20,true,true);
	return 0;	
} 

int SalidaBanco(){

	SetLightPID(0.75,0.01,3.0);
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(380),0,75,10,20,true,true);
	StraightbyGyroDegrees(CalculateTravelDegrees(110),0,20,true,true);
	//golpe de cadera para ajustar con el motor C
	OutputTimeSync(OUT_BC, 50, SPIN_LEFT, 0);
	Wait(500);

	/* 
	//opcion solo motor izquierdo
	OnFwdReg(my_robot.RightMotorPort, 60);
	Wait(500);
	Off(my_robot.RightMotorPort);*/
	// StraightbyGyroDegrees(CalculateTravelDegrees(50),0,60,true,true);
	StraightbyGyroDegrees(CalculateTravelDegrees(240),0,-60,true,true);
	//PlayTone(TONE_B2,2000);
		return 0;	
} 

int SalidaMiguel_G(){
/* 	SetLightPID(0.5f,0.001f,2.0f) con 30 de velocidad bien 
	SetLightPID(0.5f,0.001f,3.0f)     30 una pizca mejor 
*/	

	int armAngle;
	int angle;

	LcdClean();
	ResetEV3GyroSensor(my_robot.GyroPort, EV3GyroSoftwareOffset);
	ResetPowerCounters();
	ResetCounterMotor(my_robot.ArmAMotorPort);
	SetLightPID(0.75,0.0001,1.0);
	SetLightPID(0.5f,0.001f,2.0f);
	//SetLightPID(0.15,0.0001,1.0);
    SetLightPID(0.3f,0.001f,3.0f);

	SetStraightPID(1.5f, 0.0f, 0.0f);
	ResetRotationCount(my_robot.ArmAMotorPort);
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 60, 40, 1000, true);
	// Nueva Salida
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(360),0,60,10,10,true,true); //avanzar
	/*int rotationsLeft = MotorRotationCount(my_robot.LeftMotorPort);
	int rotationsRight = MotorRotationCount(my_robot.RightMotorPort);
	int traveled = abs((int)((rotationsLeft + rotationsRight) / 2));*/
	//LcdTextf(1, 0, LcdRowToY(2), "Avance %d %d", avanzar, traveled);
	TurnGyro(-90, 30, 40);
	TurnGyro(-90, 35, 20); //giro -90
	StraightbyGyroDegreesWithAccel(CalculateTravelDegrees(51),-90,40,10,true,false); // Avanzamos hasta encontrar la linea
	FollowLineDegrees(CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 30, EDGELEFT, true);
	//StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(600),-90,60,20,20,true,false);
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, -90, 15, 8, true);
	// Aqui queda hecha la prueba cooperativa
	
    //Bloque canasta
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, -60, 40, 1000, true); //Primero bajamos el elevador
	StraightbyGyroDegrees(18,-90,10,false,true);	//retrocedemos una miaja
	TurnGyro(-135, 30, 30); // girar hasta canasta
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(50),-135,30,10,true,true); //avanzar hasta canasta
	ResetRotationCount(my_robot.ArmAMotorPort);
	TurnGyro(-135, 30, 30); //ajustar angulo
	//waitButton();// Prueba canasta ver que el robot encara bien la canasta
	//Wait(3000);
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 80, 2795, 4000,true); //subir brazo izq (1000)
	ResetCounterMotor(my_robot.ArmAMotorPort);
	Wait(100);
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, -80, 2222, 4000, true); //800//false, 1100); //Altura para el lanzador de boccia
	ResetRotationCount(my_robot.ArmAMotorPort);
	//bajar brazo der
	Float(my_robot.ArmAMotorPort);
	ResetRotationCount(my_robot.ArmAMotorPort);

	// fin canasta
	
	// ir a boccia

	//StraightbyGyroDegreesWithAccel(CalculateTravelDegrees(50),-90,40,10,true,true);
	//Wait(5000);

	//waitButton();
	LcdTextf(1, 0, LcdRowToY(1), "arm_fin...%d",armAngle);
	// Fin prueba canasta
	//Camino Bocchia
	ResetPowerCounters();
	//StraightbyGyroDegrees(CalculateTravelDegrees(230),-135,-10,true,true);  //retroceder
	angle = ReadEV3GyroSensorAngle(my_robot.GyroPort, EV3_GYRO_SENSOR_ANGLE_MODE);
	LcdTextf(1, 0, LcdRowToY(3), "angulo_c...%d",angle);
	RevToUmbral(my_robot.ColorLeftPort,30,12,false);
	RevToUmbral(my_robot.ColorLeftPort,15,90,false);

	//StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, angle, -15, 8, false);
	//StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, angle, -15, 90, true);
	//Estrategia giro 15 y siguelineas	

	TurnGyro(15, 60, 40); //girar a 0 hacia otras pruebas // hay que ver la histeresis del sensor de giro
	Wait(200);
	TurnGyro(15, 40, 30);
	StraightbyGyroDegreesToUmbral(my_robot.ColorRightPort,15,20,80, false);
	StraightbyGyroDegreesToUmbral(my_robot.ColorRightPort,15,20,10, true);
	TurnGyro(0, 40, 30);
	FollowLineToUmbral(my_robot.ColorRightPort, 50, 30, my_robot.ColorLeftPort, 10, EDGELEFT,true, false);
	//FollowLineToUmbral(my_robot.ColorRightPort, 50, 30, my_robot.ColorLeftPort, 10, EDGELEFT,true, true);
	//waitButton();
	//FollowLineDegrees(CalculateTravelDegrees(150), my_robot.ColorRightPort, 50, 30, EDGELEFT, true);
	TurnGyro(0, 40, 30);
	//StraightbyGyroDegreesToUmbral(my_robot.ColorRightPort,15,20,10, false);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(180), 0, 20,10,false, true);
	angle = ReadEV3GyroSensorAngle(my_robot.GyroPort, EV3_GYRO_SENSOR_ANGLE_MODE);
	LcdTextf(1, 0, LcdRowToY(5), "angulo_fin...%d",angle);	
	// subir brazo
	TurnGyro(-15,30,30);
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 80, 300, 4000,true); 
	armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 80, -300, 4000,true);
	 TurnGyro(0,30,30);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(450),0,-60,-10,-15,true,true); 
	TurnGyro(15, 30, 30);
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(900),15,-60,-10,-15,true,true); 

	
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
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(1045),0,40,10,false,true); //Empujar cuennta pasos
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(75),0,-20,-10,-10,true,true); //Retroceder 10 cm
	TurnGyro(-90,40,30);
	OnFwdEx(my_robot.ArmBMotorPort,100,RESET_NONE); // inicio bombeo aire
    StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(300),-90,75,5,60,true,false);
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, -90,60,80,false);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(260), -90, 75, 5, true, true); //llegada a Boccia
	// Aqui empieza el rocknroll
	//OnFwdEx(my_robot.ArmBMotorPort,150,RESET_NONE);
	MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 525, 2000, true); //Calculates ratio por 63 grados de apertura del switch neumatico
	ResetCounterMotor(my_robot.ArmAMotorPort);
	Wait(2500); // subir teleferico y tirar bolas Boccia
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
	 
  	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1000),0,75,20,20,true,false); //1080
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(1110),0,20,10,false,true);
	/* Soltamos el bloqueador del pendulo 
	dejamos caer por gravedad quitando el pestillo el gancho de coger el corazon */
    int brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 100, 2000, true); 
//	LcdTextf(1, 0, LcdRowToY(1), "arm_fin...%d",brazo);
	// Wait(2000);
    //StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(450),0,75,10,10,true,false);//300
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(300),0,75,10,40,true,false);//300
	StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort,0,40,10,false);
	StraightbyGyroDegreesWithBrake(CalculateTravelDegrees(20),0,40,10,true,true);
	TurnGyro(0,40,30);
	OnFwdEx(my_robot.ArmBMotorPort,100,RESET_NONE);
	Wait(5000);
	Off(my_robot.ArmBMotorPort); //fin rueda
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(80),0,-30,-10,-10,true,true);
	//si eso giro del brazo a para coger corazon
	brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 360, 1500, true); 
	//waitButton();
	TurnGyro(-38,30,30); // Giro para encarar la rueda de arrastre
	//brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, 100, 200, 2000, true); 
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(220),-38,40,10,10,true,true); //avanzo hasta rueda
	brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, -100, 360, 1500, true); //pillo rueda
	StraightbyGyroDegrees(CalculateTravelDegrees(40),-38,-20,true,true);//retrocedo un poco
	//waitButton();
	TurnGyro(-72,40,30);// para poner en el circulo
	Wait(500); // pausa para freno
	StraightbyGyroDegrees(CalculateTravelDegrees(20),-72, 10,true,true); //Ajusto al circulo
	// en el circulo
	ResetCounterMotor(my_robot.ArmAMotorPort);
	brazo = MoveArmTimeProtected(my_robot.ArmAMotorPort, 60, 200, 1500, true); //brazo arriba
	StraightbyGyroDegrees(CalculateTravelDegrees(30),-72, -10,true,true);
	//StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(180),-68,-40,-10,-10,true,true);
	TurnGyro(0,40,30); 
	StraightbyGyroDegreesWithRamps(CalculateTravelDegrees(1610),0,-75,-10,-10,true,true);



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
/*	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawLeft);
	ReadEV3ColorSensorRawReflectedLight(my_robot.ColorLeftPort, &lightRawRight); */
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
	// CalibrateLight();
	
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
	Wait(3000);
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
	LcdClean();
	ResetPowerCounters();
	RevToUmbral(my_robot.ColorLeftPort,15,8,false);
	//StraightbyGyroDegreesToUmbral(my_robot.ColorLeftPort, 0, -15, 8, true);
	return 0;
	
	SetLightPID(2.0,0.01,1.0);
	SetLightPID(2,0.001,0.0);
	SetLightPID(0.5,0.001,0.01);
	SetLightPID(2,5,1);
	//TurnToBlack();
	TurnToBlack(TO_RIGHT, my_robot.ColorRightPort, 50, true);
	SetTimerMS(1,0);
	
	int itera = FollowLineDegreesdevel( CalculateTravelDegrees(1000), my_robot.ColorRightPort, 50, 40, EDGELEFT, true);
	long tiempo = TimerMS(1);

	LcdTextf(1, 0, LcdRowToY(2), "Itera: %d",itera);
	LcdTextf(1, 0, LcdRowToY(4), "Tiempo: %ld",tiempo);

	Wait(6000);
return 0;	
}
int Aux4() {
	//ResetPowerCounters();
	//SetLightPID(1.0,0.01,3.0);
	//FollowLineDegrees( CalculateTravelDegrees(600), my_robot.ColorRightPort, 50, 40, false, true);
//OnFwdReg(my_robot.RightMotorPort, 30);
//Wait(500);
//Off(my_robot.RightMotorPort);
int armAngle;
LcdClean();
	ResetCounterMotor(my_robot.ArmAMotorPort);
	ResetRotationCount(my_robot.ArmAMotorPort);
armAngle = MotorRotationCount(my_robot.ArmAMotorPort);	
LcdTextf(1, 0, LcdRowToY(2), "Inicio real: %d",armAngle);
armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, 20, 1000, 4000, true);

LcdTextf(1, 0, LcdRowToY(4), "Sube 360, real: %d",armAngle);
armAngle=MoveArmTimeProtected(my_robot.ArmAMotorPort, -20, 1000, 4000, true);
LcdTextf(1, 0, LcdRowToY(6), "Baja 360, real: %d",armAngle);
Wait(5000);
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
	CreateMenuKeys(1, SalidaColumpio, Aux1, Aux2, Salida1, "Salida Columpio","Reset Sensor Giro","Calibrate Color","Comprobar cal");

	MenuButtons();
	LcdClean();
	FreeEV3();
	return 0;
}
