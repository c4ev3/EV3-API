#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the Pixy Cam 1 sensor.
 * The program prints every 100ms the biggest blob found. To use this program you first
 * need to select a blob connecting the PixyCam to a computer.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize PixyCam sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(PixyCam, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        PixyRectangle blob;
        ReadPixyCamSensorBiggestBlobRectangle(IN_1, &blob);

        LcdClean();
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Signature: %d", blob.signature);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(2), "X: %d\tY: %d", blob.x, blob.y);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(3), "Width: %d\tHeight: %d", blob.width, blob.height);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(4), "Angle: %d", blob.angle);
        Wait(100);
    }

    FreeEV3();
    return 0;
}
