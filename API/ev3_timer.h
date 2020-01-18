/** \file ev3_timer.h
 * \brief Functions and constants for controlling EV3 timers
 *
 * ev3_timer.h contains declarations for the EV3 C API timer functions.
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
