#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the NXT temperature sensor.
 * The programs check every 100ms the temperature and prints it to the screen.
 * Stop the program pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize NXTTemperature sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(NXTTemperature, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        LcdClean();

        int c = ReadNXTTemperatureSensor(IN_1, C);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "C: %d", c);

        int f = ReadNXTTemperatureSensor(IN_1, F);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(2), "F: %d", f);

        Wait(100);
    }

    FreeEV3();
    return 0;
}
