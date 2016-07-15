/** \file ev3.h
 * \brief Initialisation for all working EV3 functions (sensors, lcd, motors, buttons)
 *
 * ev3.h contains declarations for the EV3 C API Initialisationfunctions.
 *
 * License:
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is Simón Rodriguez Perez.
 * Portions created by Simón Rodriguez Perez are Copyright (C) 2014-2015 Simón Rodriguez Perez.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Simón Rodriguez Perez (Hochschule Aschaffenburg)
 * \date 2015-05-28
 * \version 1
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_h
#define ev3_h

#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#include <ev3_constants.h>
#include <ev3_command.h>
#include <ev3_output.h>
#include <ev3sensor.h>
#include <ev3_button.h>
#include <ev3_lcd.h>
#include <ev3_sound.h>

int InitEV3(void);
int CloseEV3(void);
int ExitEV3(void);
int FreeEV3(void);

#endif // ev3_h

#ifdef __cplusplus
}
#endif
