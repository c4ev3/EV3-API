/** \file ev3_timer.h
 * \brief Functions and constants for controlling EV3 timers
 *
 * ev3_timer.h contains declarations for the EV3 C API timer functions.
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
 * \date 2013-07-10
 * \version 1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_timer_h
#define ev3_timer_h

#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>

#ifndef DISABLE_TIMERS

#include "ev3_constants.h"

typedef enum {
    ti10ms,
    ti50ms,
    ti100ms,
    ti250ms,
    ti500ms,
    ti1sec
} TimerInterval;

typedef void (*TimerCallback)(int sig);


unsigned long TimerWait(unsigned long Time);
void TimerReady(unsigned long Timer);

unsigned long Timer(uint8_t Timer);
unsigned long FastTimer(uint8_t Timer);
void ClearTimer(uint8_t Timer);
void SetTimer(uint8_t Timer, unsigned long Value);

unsigned long TimerMS(uint8_t Timer);
void ClearTimerMS(uint8_t Timer);
void SetTimerMS(uint8_t Timer, unsigned long Value);

unsigned long TimerUS(uint8_t Timer);
void ClearTimerUS(uint8_t Timer);
void SetTimerUS(uint8_t Timer, unsigned long Value);

void TimerInit(unsigned long interval_multiplier);

void SetTimerCallback(TimerInterval interval, TimerCallback callback);

#endif // !DISABLE_TIMERS

unsigned long long TimerGetUS();
unsigned long long TimerGetMS();
unsigned long long TimerGetCS();

#endif // ev3_timer_h

#ifdef __cplusplus
}
#endif
