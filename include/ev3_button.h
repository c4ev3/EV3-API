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

float HardwareVersion();

const char* HardwareVersionString();

#define LED_BLACK        0  //!< LED black pattern
#define LED_GREEN        1  //!< LED green pattern
#define LED_RED          2  //!< LED red pattern
#define LED_ORANGE       3  //!< LED orange pattern
#define LED_GREEN_FLASH  4  //!< LED green flash pattern
#define LED_RED_FLASH    5  //!< LED red flash pattern
#define LED_ORANGE_FLASH 6  //!< LED orange flash pattern
#define LED_GREEN_PULSE  7  //!< LED green pulse pattern
#define LED_RED_PULSE    8  //!< LED red pulse pattern
#define LED_ORANGE_PULSE 9  //!< LED orange pulse pattern
#define NUM_LED_PATTERNS 10 //!< The number of LED patterns

void SetLedWarning(bool Value);

bool IsLedWarning();

void SetLedPattern(uint8_t Pattern);

uint8_t GetLedPattern();

uint16_t ButtonWaitForAnyEvent(unsigned int timeout);

uint16_t ButtonWaitForAnyPress(unsigned int timeout);

bool ButtonIsUp(uint8_t Button);

bool ButtonIsDown(uint8_t Button);

void ButtonWaitForPress(uint8_t Button);

void ButtonWaitForPressAndRelease(uint8_t Button);

// NXC-style API functions (no support for short press, long press,
// short release, long release, or press counts
bool ButtonPressedEx(uint8_t btn, bool resetCount);

static inline bool ButtonPressed(uint8_t btn) {
    return ButtonPressedEx(btn, false);
}

char ReadButtonEx(uint8_t btn, bool reset, bool* pressed, uint16_t* count);

uint8_t ButtonState(uint8_t btn);

#endif // ev3_button_h

#ifdef __cplusplus
}
#endif
