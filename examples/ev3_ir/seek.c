#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the EV3 IR sensor.
 * The program checks every 100ms the presence of a beacon and its position.
 * Stop the program by pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize EV3Ir sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(EV3Ir, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        LcdClean();

        EV3IrSeekResult result = ReadEV3IrSensorSeek(IN_1);

        int i;
        for (i = 0; i < EV3_IR_CHANNELS; i++) {
            LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(i + 1), "CH %d: %d %d cm", (i + 1), result.directions[i], result.distances[i]);
        }

        Wait(100);
    }

    FreeEV3();
    return 0;
}
