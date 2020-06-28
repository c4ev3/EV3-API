#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the HiTechnic Color Sensor V2.
 * The program prints the color code every 200ms.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Color sensor connected at port 1 for a country that uses 60Hz mains power
     */
    InitEV3();
    SetAllSensors(HTColorV2, NULL, NULL, NULL);
    SetHTColorV2PowerMainsFrequency(IN_1, PowerMains60Hz);

    while (!isExitButtonPressed()) {
        int color = ReadHTColorSensorV2(IN_1);

        LcdClean();
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Color: %d", color);
        Wait(200);
    }

    FreeEV3();
    return 0;
}
