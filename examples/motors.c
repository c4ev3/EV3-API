#include "ev3.h"

int main () {
    InitEV3();

    // Go forward using 2 synchronized motors for 3s
    // at speed 30 and then stop
    OnFwdSync(OUT_BC, 30);
    Wait(3000);
    Off(OUT_BC);

    // Go forward using 2 synchronized motors to reach a certain angle
    // at speed 30 and then stop
    RotateMotor(OUT_BC, 30, 180); // 180 = half rotation of the motor

    // Move one motor for 1s at speed 30 and then let the motor slowly stop
    OnFwdReg(OUT_A, 30);
    Wait(1000);
    Float(OUT_A);

    FreeEV3();
    return 0;
}