#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

bool isCenterButtonPressed() {
    return ButtonIsDown(BTNCENTER);
}

/**
 * Example program that uses the EV3 gyro sensor.
 * The program reads the angle and shows it on the screen every 100ms.
 * Press the center button to reset the angle (please, keep the robot still
 * while resetting, to avoid the common drifting problem).
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Gyro sensor connected at port 1
     */
    InitEV3();
    SetAllSensorMode(GYRO_ANG, NO_SEN, NO_SEN, NO_SEN);

    while (!isExitButtonPressed()) {
        int angle = ReadSensor(IN_1);

        LcdClean();
        LcdTextf(1, 0, LcdRowToY(1), "Angle: %d", angle);
        Wait(100);

        if (isCenterButtonPressed()) {
            LcdClean();
            LcdTextf(1, 0, 8, "Resetting gyro sensor ...");
            Wait(1000);
            ResetEV3GyroSensor(IN_1, EV3GyroSoftwareOffset);
        }
    }

    FreeEV3();
    return 0;
}
