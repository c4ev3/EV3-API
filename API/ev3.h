/** \file ev3.h
 * \brief Initialisation for all working EV3 functions (sensors, lcd, motors, buttons)
 *
 * ev3.h contains declarations for the EV3 C API Initialisationfunctions.
 *
 * License:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
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

#include <ev3_constants.h>
#include <ev3_command.h>
#include <ev3_output.h>
#include <ev3_sensor.h>
#include <ev3_button.h>
#include <ev3_lcd.h>
#include <ev3_sound.h>

// Priority of the InitEV3/FreeEV3 functions for automatic construction and destruction
// see https://gcc.gnu.org/onlinedocs/gcc-4.3.3/gcc/Function-Attributes.html
// see https://stackoverflow.com/a/24361145
// - 0-100 is reserved for the compiler
// - 101-200 is left for the user
#define EV3_CONSTRUCTOR_PRIORITY 201
#define EV3_DESTRUCTOR_PRIORITY 201

int InitEV3(void);
int FreeEV3(void);
bool EV3IsInitialized(void);

/*
 *  Compatibility alias definitions
*/

/*!
 * @deprecated
*/
#define CloseEV3() FreeEV3()

/*!
 * @deprecated
*/
#define ExitEV3() FreeEV3()

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
