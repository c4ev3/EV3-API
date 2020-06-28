#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

bool isCenterButtonPressed() {
    return ButtonIsDown(BTNCENTER);
}

/**
 * Example program that uses the HiTechnic Compass Sensor.
 * The program prints the angle of the compass sensor.
 *
 * Press the center button to calibrate the sensor. Once the calibration is started,
 * you should rotate very slowly the sensor, taking at least 20 seconds per rotation.
 * You should make 1.5 to 2 full rotations. After you completed the calibration,
 * press the center button again.
 *
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Color sensor connected at port 1 for a country that uses 60Hz mains power
     */
    InitEV3();
    SetAllSensors(HTCompass, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        int angle = ReadHTCompassSensor(IN_1, HTCompassAngle);

        LcdClean();
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Angle: %d", angle);

        if (isCenterButtonPressed()) {
            LcdClean();
            LcdTextf(1, 0, 8, "Start calibration");
            Wait(1000);
            StartHTCompassCalibration(IN_1);
            while(!isCenterButtonPressed()) {
                Wait(100);
            }
            StopHTCompassCalibration(IN_1);
            LcdClean();
            LcdTextf(1, 0, 8, "Done");
            Wait(1000);
        }
    }

    FreeEV3();
    return 0;
}
