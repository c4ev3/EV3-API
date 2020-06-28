#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the HiTechnic Color Sensor V2.
 * The program prints the rgb values every 200ms.
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
        RGBA rgba;

        ReadHTColorSensorV2RGBA(IN_1, HTColorSensorDefaultMode, &rgba);
        //ReadHTColorSensorV2RGBA(IN_1, HTColorSensorPassiveMode, &rgba);
        //ReadHTColorSensorV2RGBA(IN_1, HTColorSensorRawMode, &rgba);

        LcdClean();
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "W:\tR: %d\tG: %d\tB: %d", rgba.white, rgba.red, rgba.green, rgba.blue);
        Wait(500);
    }

    FreeEV3();
    return 0;
}
