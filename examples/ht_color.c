#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the HiTechnic Color Sensor V2.
 * The program prints the color and then the rgba value in the three different modes.
 * The sensor automatically change its mode according to the reading function/parameters.
 * When the sensor changes its mode, the program automatically waits 200ms.
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
        LcdClean();

        int color = ReadHTColorSensorV2(IN_2);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Color: %d", color);

        RGBA rbaDefault = ReadHTColorSensorV2RGBA(IN_2, HTColorSensorDefaultMode);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(2), "Default:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(3), "W:\tR: %d\tG: %d\tB: %d", rbaDefault.white, rbaDefault.red, rbaDefault.green, rbaDefault.blue);

        RGBA rbaRaw = ReadHTColorSensorV2RGBA(IN_2, HTColorSensorPassiveMode);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(4), "Passive:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(5), "W:\tR: %d\tG: %d\tB: %d", rbaRaw.white, rbaRaw.red, rbaRaw.green, rbaRaw.blue);

        RGBA rbaPassive = ReadHTColorSensorV2RGBA(IN_2, HTColorSensorRawMode);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(6), "Raw:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(7), "W:\tR: %d\tG: %d\tB: %d", rbaPassive.white, rbaPassive.red, rbaPassive.green, rbaPassive.blue);
    }

    FreeEV3();
    return 0;
}
