# Fork of EV3-API [![Build Status](https://travis-ci.org/simonedegiacomi/EV3-API.svg?branch=master)](https://travis-ci.org/simonedegiacomi/EV3-API)

This API supports the following functionalities:
* Controlling LEGO motors
* Reading from the following sensors:
    - Touch
    - Ultrasonic
    - Color
    - Gyro
    - Infrared
    - HiTechnic Color Sensor V2
    - HiTechnic Compass Sensor
    - HiTechnic Infrared Sensor
    - NXT Temperature
    - NXT Sound
    - Pixy Cam
* Controlling Buttons and LEDs
* Printing text on LCD
* Playing sounds and audio files

## Requirements

In order to compile a program that uses this library, or to rebuild this library, you need a compatible ARM toolchain.
You can download the one from the [official c4ev3 repository](https://github.com/c4ev3/C4EV3.Toolchain/releases).

## Using the library

Download the static library form one of the two links, according to the toolchain you are using:
- [libev3api.a uclibc](https://simonedegiacomi.github.io/EV3-API/uclibc/libev3api.a)
- [libev3api.a glibc](https://simonedegiacomi.github.io/EV3-API/glibc/libev3api.a)

> If you are using the toolchain from c4ev3, download the uclibc version.

Create now a new file:
```c
// hello.c

#include <ev3.h>

int main () {
    InitEV3();

    LcdTextf(LCD_COLOR_BLACK, 0, 0, "Hello world!");
    Wait(5000);

    FreeEV3();
    return 0;
}
```

Then compile it with:
```bash
arm-c4ev3-linux-uclibcgnueabi-gcc hello.c -I /path/to/c4ev3/include -L /path/to/static/library -lev3api
```

Finally, use [ev3duder](https://github.com/c4ev3/ev3duder) to upload the program to the robot.

## Documentation

You can find the documentation here: [https://simonedegiacomi.github.io/EV3-API/files.html](https://simonedegiacomi.github.io/EV3-API/files.html)