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
#include <stdbool.h>

#ifndef DISABLE_TIMERS

#include "../include/ev3_constants.h"

typedef enum {
    ti10ms,
    ti50ms,
    ti100ms,
    ti250ms,
    ti500ms,
    ti1sec
} TimerInterval;

typedef void (*TimerCallback)(int sig);

/**
 * Creates a new timer that will expire after the specified amount of time.
 * Call @ref TimerReady to wait for the times to elapse
 * @param Time in ms
 * @return
 */
unsigned long TimerWait(unsigned long Time);

/**
 * Blocks until the time has elapsed
 * @param Timer time to wait for
 */
void TimerReady(unsigned long Timer);

/**
 * Return the value of the timer
 * @param timerId Id of the timer, between 0 and (@ref NUM_MS_TIMERS - 1)
 * @return
 */
unsigned long TimerMS(uint8_t timerId);

/**
 * Clears the timer
 * @param timerId Id of the timer, between 0 and (@ref NUM_MS_TIMERS - 1)
 */
void ClearTimerMS(uint8_t timerId);

/**
 * Return the value of the timer
 * @param timerId Id of the timer, between 0 and (@ref NUM_US_TIMERS - 1)
 * @return
 */
unsigned long TimerUS(uint8_t timerId);

/**
 * Clears the timer
 * @param timerId Id of the timer, between 0 and (@ref NUM_US_TIMERS - 1)
 */
void ClearTimerUS(uint8_t timerId);

void TimerInit(unsigned long interval_multiplier);

/**
 * Adds a callback that will be called every specified interval
 * @param interval
 * @param callback
 * @return true if the callback was added, false otherwise
 */
bool AddTimerCallback(TimerInterval interval, TimerCallback callback);

#endif // !DISABLE_TIMERS

unsigned long long TimerGetUS();
unsigned long long TimerGetMS();

#endif // ev3_timer_h

#ifdef __cplusplus
}
#endif
