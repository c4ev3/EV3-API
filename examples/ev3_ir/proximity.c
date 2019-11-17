#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 IR sensor.
 * The program checks every 100ms the proximity.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Ir sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(EV3Ir, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        LcdClean();

        int proximity = ReadEV3IrSensorProximity(IN_1);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Proximity: %d cm", proximity);

        Wait(100);
    }

    FreeEV3();
    return 0;
}
