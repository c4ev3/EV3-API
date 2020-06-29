#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 ultrasonic sensor.
 * The program checks every 100ms the distance.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3 ultrasonic sensor connected at port 1
     */
    InitEV3();
    SetAllSensorMode(US_DIST_CM, NO_SEN, NO_SEN, NO_SEN);

    while (!isExitButtonPressed()) {
        LcdClean();

        int distance = ReadSensor(IN_1);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Distance: %d cm", distance);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
