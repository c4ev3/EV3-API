/** \file ev3_button.h
 * \brief Functions and constants for controlling EV3 buttons and LEDs
 *
 * ev3_button.h contains declarations for the EV3 C API button and LED functions.
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
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author John Hansen (bricxcc_at_comcast.net)
 * \date 2013-06-20
 * \version 1
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

char* HardwareVersionString();

void SetLedWarning(bool Value);

byte LedWarning();

void SetLedPattern(byte Pattern);

byte LedPattern();

word ButtonWaitForAnyEvent(unsigned int timeout);

word ButtonWaitForAnyPress(unsigned int timeout);

bool ButtonIsUp(byte Button);

bool ButtonIsDown(byte Button);

void ButtonWaitForPress(byte Button);

void ButtonWaitForPressAndReleae(byte Button);

// NXC-style API functions (no support for short press, long press,
// short release, long release, or press counts
bool ButtonPressedEx(byte btn, bool resetCount);

#define ButtonPressed(_btn) ButtonPressedEx((_btn), FALSE)

char ReadButtonEx(byte btn, bool reset, bool* pressed, word* count);

byte ButtonState(byte btn);

#endif // ev3_button_h

#ifdef __cplusplus
}
#endif
