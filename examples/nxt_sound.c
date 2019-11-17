#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

/**
 * Example program that uses the NXT sound sensor.
 * The programs check every 100ms the sound level and prints it to the screen.
 * Stop the program pressing the back/exit button of the robot.
 */
int main () {
    /**
     * Initialize NXTSound sensor connected at port 1
     */
    InitEV3();
    SetAllSensors(NXTSound, NULL, NULL, NULL);

    while (!isExitButtonPressed()) {
        LcdClean();

        int sound = ReadNXTSoundSensor(IN_1, NXT_SOUND_DB);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Sound: %d", sound);

        Wait(100);
    }

    FreeEV3();
    return 0;
}
