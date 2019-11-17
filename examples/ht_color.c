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
        int color = ReadHTColorSensorV2(IN_2);

        RGBA rgbaDefault, rgbaRaw, rgbaPassive;

        ReadHTColorSensorV2RGBA(IN_2, HTColorSensorDefaultMode, &rgbaDefault);
        ReadHTColorSensorV2RGBA(IN_2, HTColorSensorPassiveMode, &rgbaRaw);
        ReadHTColorSensorV2RGBA(IN_2, HTColorSensorRawMode, &rgbaPassive);

        LcdClean();
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Color: %d", color);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(2), "Default:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(3), "W:\tR: %d\tG: %d\tB: %d", rgbaDefault.white, rgbaDefault.red, rgbaDefault.green, rgbaDefault.blue);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(4), "Passive:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(5), "W:\tR: %d\tG: %d\tB: %d", rgbaRaw.white, rgbaRaw.red, rgbaRaw.green, rgbaRaw.blue);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(6), "Raw:");
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(7), "W:\tR: %d\tG: %d\tB: %d", rgbaPassive.white, rgbaPassive.red, rgbaPassive.green, rgbaPassive.blue);
    }

    FreeEV3();
    return 0;
}
