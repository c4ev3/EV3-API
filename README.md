# EV3-API
This API supports the follwing functionallity:
* Easy initialistion with one header-file
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
The work is part of the [EVELIN](http://www.evelinprojekt.de/en/) project, which was funded by the German Federal Ministry of Education and Research (Bundesministerium für Bildung und Forschung)	under grant number 01PL12022B.   
The authors are responsible	for the content of this publication.

In agreement with the university, it's hereby released under the terms of the Mozilla Public License Version 1.1 and GNU General Public License for the Sensors-API.
