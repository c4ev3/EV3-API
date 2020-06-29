#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 color sensor.
 * The program reads the color, RGB values, reflected and ambient light and prints
 * the values on the screen.
 *
 * The sensor automatically change its mode according to the reading function/parameters-
 * When the sensor changes its mode, the program automatically waits 200ms.
 *
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Color sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(EV3Color, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {

        Color color = ReadEV3ColorSensorColor(IN_1);

        RGB rgb;
        int res = ReadEV3ColorSensorColorRGB(IN_1, &rgb);

        int reflectedLight = ReadEV3ColorSensorReflectedLight(IN_1);

        int ambientLight = ReadEV3ColorSensorAmbientLight(IN_1);


        LcdClean();
        LcdTextf(1, 0, LcdRowToY(1), "Color: %d", color);
        LcdTextf(1, 0, LcdRowToY(2), "R: %d\tG: %d\tB: %d", rgb.red, rgb.green, rgb.blue);
        LcdTextf(1, 0, LcdRowToY(3), "Reflected: %d", reflectedLight);
        LcdTextf(1, 0, LcdRowToY(4), "Ambient: %d", ambientLight);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
