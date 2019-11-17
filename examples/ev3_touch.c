#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 touch sensor.
 * The program checks every 100ms if a touch sensor connected to port 1 is pressed
 * and write if it is pressed on the screen. Stop the program by pressing the back/exit
 * button of the robot.
 */
int main () {
    /**
     * Initialize EV3Touch sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(EV3Touch, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {

        /**
         * Read if the button is pressed (true) or not (false)
         */
        bool touchSensor = ReadEV3TouchSensor(IN_1);

        LcdClean();
        if (touchSensor) {
            LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Button is pressed");
        } else {
            LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Button is not pressed");
        }
        Wait(100);
    }

    FreeEV3();
    return 0;
}
