# EV3-API [![Build Status](https://travis-ci.org/c4ev3/EV3-API.svg?branch=master)](https://travis-ci.org/c4ev3/EV3-API) [![Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/c4ev3/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

This API supports the follwing functionallity:
* Easy initialization with one header-file
* Controlling LEGO motors
* Reading out of Touch, Ultrasonic, Color, Gyro, EV3-Infrared, NXT-Infrared Sensor and NXT Temperaturesensor
* Controling Buttons and LED
* Printing text on LCD
* Playing sounds

The basis of this API was developed by [John Hansen](http://bricxcc.sourceforge.net/test_releases/)
and [Carsten Zeiffert](https://github.com/carstenz/ev3sensor/).
These API's were merged and extended.

## Building from source
If you change something in the source, rebuild with a simple `make` (with MinGW, it's called `mingw32-make`).

Unless you are using the Eclipse Plugin, you will also need to explicitly link against the static `.a` file generated like so:
`-L/path/to/libev3api -lev3api`.

# Acknowledgements
This plugin has been originally written for use in the teaching of Informatics at the [Hochschule Aschaffenburg](https://www.h-ab.de/).
The work is part of the [EVELIN](http://www.evelinprojekt.de/en/) project, which was funded by the German Federal Ministry of Education and Research (Bundesministerium für Bildung und Forschung) under grant number 01PL12022B.
The authors are responsible for the content of this publication.

The code is provided under the terms of the GNU General Public License v2.0.
