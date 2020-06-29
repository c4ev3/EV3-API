#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 color sensor.
 * The program reads the ambient light and prints the value on the screen.
 *
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Color sensor connected at port 1
     */
    InitEV3();
    SetAllSensorMode(COL_AMBIENT, NO_SEN, NO_SEN, NO_SEN);

    while (!isExitButtonPressed()) {
        int ambient = ReadSensor(IN_1);

        LcdClean();
        LcdTextf(1, 0, LcdRowToY(1), "Ambient: %d", ambient);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
