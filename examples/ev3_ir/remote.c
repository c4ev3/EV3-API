#include <ev3.h>

bool isExitButtonPressed() {
    return ButtonIsDown(BTNEXIT);
}

const char* getBeaconButtonName (int beaconButton) {
    switch (beaconButton) {
        case BEACON_OFF:
            return "Off";
        case BEACON_UP_LEFT:
            return "Up Left";
        case BEACON_DOWN_LEFT:
            return "Down Left";
        case BEACON_UP_RIGHT:
            return "Up Right";
        case BEACON_DOWN_RIGHT:
            return "Down Right";
        case BEACON_UP:
            return "Both Up";
        case BEACON_DIAG_UP_LEFT:
            return "Down Right + Up Left";
        case BEACON_DIAG_UP_RIGHT:
            return "Down Left + Up Right";
        case BEACON_DOWN:
            return "Both Down";
        case BEACON_ON:
            return "On";
        case BEACON_LEFT:
            return "Both Left";
        case BEACON_RIGHT:
            return "Both Right";
        default:
            return "None";
    }
}

/**
 * Example program that uses the EV3 IR sensor.
 * The program checks every 100ms if and which EV3 beacon button is pressed.
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

        // read pressed button of beacon transmitting on channel 1
        int beaconButton = ReadEV3IrSensorRemote(IN_1, BEACON_CH_1);
        const char* buttonName = getBeaconButtonName(beaconButton);
        LcdTextf(LCD_COLOR_BLACK, 0, LcdRowToY(1), "Beacon button: %s", buttonName);

        Wait(100);
    }

    FreeEV3();
    return 0;
}
