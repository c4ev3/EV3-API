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
    SetAllSensors(EV3Ultrasonic, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        LcdClean();

        int distance = ReadEV3UltrasonicSensorDistance(IN_1, CM);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Distance: %d cm", distance);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
