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
	.Kfriction = 1.0f
};



bool isCenterButtonPressed() {
    return ButtonIsDown(BTNCENTER);
}

/**
 * Example program that uses the EV3 Robot libray for FLL.
 * The program initializes the robot, and travel 100 cm in straight
 * Initializes the robot in debug mode, and then press center key to move
 *
 *  The program stops at end.
 *  version 0.0.1
 */
int main () {
    /**
     * Initialize EV3Color sensor connected at port 1
     */
    LcdClean();
    InitEV3();
    SetAllSensors(EV3GYRO, EV3COLOR, EV3COLOR, NULL);
    RobotInit(&my_robot, true);

    while (!isCenterButtonPressed()) {
//do nothing
    }
    int travelDegrees = CalculateTravelDegrees(1000);
    int travel =  StraightbyGyroDegrees(travelDegrees, 0, 60, true)  // move traveldegrees (1meter), in head direction, at 60 spped and stop at end

    FreeEV3();
    return 0;
}
