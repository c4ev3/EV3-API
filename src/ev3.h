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
#include <stdbool.h>

#include "ev3_constants.h"
#include "ev3_timer.h"
#include "ev3_wait.h"
#include "ev3_output.h"
#include "ev3_sensors/ev3_sensors.h"
#include "ev3_button.h"
#include "ev3_lcd.h"
#include "ev3_sound.h"
#include "ev3_bluetooth.h"


int InitEV3(void);
int FreeEV3(void);
bool EV3IsInitialized(void);



/* Students are lazy, so lets have the intialization happen automatically for them.
 * This code is usually linked in as a static library and the linker will throw
 * the InitEV3 function out if no one wants it, so lets reference it manually
 * and have GCC warn if a user accesses it.
 */

#ifndef EV3_NO_CONSTRUCTORS
static int __attribute__((used,deprecated)) (* volatile __reference_InitEV3)(void) = InitEV3;
static int __attribute__((used,deprecated)) (* volatile __reference_FreeEV3)(void) = FreeEV3;
#endif

#endif // ev3_h

#ifdef __cplusplus
}
#endif
