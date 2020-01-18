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

In agreement with the university, it's hereby released under the terms of the Mozilla Public License Version 1.1 and GNU General Public License 3.0 for the Sensors-API.

# Binary Releases and Relicensing

As the GNU GPL and the MPL 1.1 are incompatible, redistribution of compiled artifacts is a license violation. We thus no longer distribute pre-built binaries. Users may still build the software themselves for personal use. We hope to relicense the MPL 1.1 parts under the MPL 2.0, so we can continue to distribute pre-built packages. See [Issue #34](https://github.com/c4ev3/EV3-API/issues/34) for more information.
