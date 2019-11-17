#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 ultrasonic sensor.
 * The program checks every 100ms the presence of another ultrasonic sensor.
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

        bool presence = ReadEV3UltrasonicSensorListen(IN_1);
        if (presence) {
            LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Presence of another sensor");
        } else {
            LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "No other sensors");
        }
        Wait(100);
    }

    FreeEV3();
    return 0;
}
