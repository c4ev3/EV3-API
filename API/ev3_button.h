/** \file ev3_button.h
 * \brief Functions and constants for controlling EV3 buttons and LEDs
 *
 * ev3_button.h contains declarations for the EV3 C API button and LED functions.
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
 * The Initial Developer of this code is John Hansen.
 * Portions  by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author John Hansen (bricxcc_at_comcast.net)
 * \date 2013-06-20
 * \version 1
 * 
 * ----------------------------------------------------------------------------
 *
 * \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2016-04-19
 * \version 2
 * \note Correction of function name [void ButtonWaitForPressAndRelease(byte Button)]
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_button_h
#define ev3_button_h

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <limits.h>

#include "ev3_constants.h"

bool ButtonLedInit();

bool ButtonLedOpen();

bool ButtonLedClose();

bool ButtonLedExit();

bool ButtonLedInitialized();

float HardwareVersion();

const char* HardwareVersionString();

void SetLedWarning(bool Value);

uint8_t LedWarning();

void SetLedPattern(uint8_t Pattern);

uint8_t LedPattern();

uint16_t ButtonWaitForAnyEvent(unsigned int timeout);

uint16_t ButtonWaitForAnyPress(unsigned int timeout);

bool ButtonIsUp(uint8_t Button);

bool ButtonIsDown(uint8_t Button);

void ButtonWaitForPress(uint8_t Button);

void ButtonWaitForPressAndRelease(uint8_t Button);

// NXC-style API functions (no support for short press, long press,
// short release, long release, or press counts
bool ButtonPressedEx(uint8_t btn, bool resetCount);

#define ButtonPressed(_btn) ButtonPressedEx((_btn), false)

char ReadButtonEx(uint8_t btn, bool reset, bool* pressed, uint16_t* count);

uint8_t ButtonState(uint8_t btn);

#endif // ev3_button_h

#ifdef __cplusplus
}
#endif
