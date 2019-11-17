#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 gyro sensor.
 * The program reads the rate and shows it on the screen every 100ms.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Gyro sensor connected at port 1
     */
    InitEV3();
    SetAllSensorMode(GYRO_RATE, NO_SEN, NO_SEN, NO_SEN);

    while (!isExitButtonPressed()) {
        int angle = ReadSensor(IN_1);

        LcdClean();
        LcdTextf(1, 0, LcdRowToY(1), "Rate: %d", angle);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
