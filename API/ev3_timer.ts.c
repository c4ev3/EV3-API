/** \file ev3_timer.ts.c
 * \brief Implementation of functions relating to timestamp slots
 *
 * License:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek (linuxtardis_at_gmail.com)
 * \date 2020-02-13
 * \version 2
 * \note split into a separate file, added better error handling & locking
 */

#include "ev3.h"
#include "ev3_timer.private.h"

////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/*!
 * \brief Safely write a given value to a given timestamp slot.
 * \param timerArray Timestamp slot array to access.
 * \param timerIndex Index to the array.
 * \param maxTimers Number of the available slots in the array.
 * \param value Value to write.
 * \param getNow How to get the current time (for offset calculation).
 */
static
void tmTimerWrite(unsigned long long *timerArray,
                  uint8_t timerIndex,
                  uint8_t maxTimers,
                  unsigned long long value,
                  unsigned long long (*getNow)(void));

/*!
 * \brief Safely retrieve a given timestamp slot value.
 * \param timerArray Timestamp slot array to access.
 * \param timerIndex Index to the array.
 * \param maxTimers  Number of available slots in the array.
 * \param getNow How to get current time (for offset calculation).
 * \return Timestamp value or zero.
 */
static
unsigned long long tmTimerRead(const unsigned long long int *timerArray,
                               uint8_t timerIndex,
                               uint8_t maxTimers,
                               unsigned long long (*getNow)(void));

////////////////////////////////////////////////////////////////
// IMPLEMENTATION


// INITIALIZATION //

void tmInitTimestampSlots(void)
{
    unsigned long long csTick, msTick, usTick;
    int i;

    usTick = TimerGetUS();
    msTick = usTick / 1000ULL;
    csTick = msTick / 10ULL;

    for (i = 0; i < NUM_US_TIMERS; i++)
        tcb.usTimers[i] = usTick;
    for (i = 0; i < NUM_MS_TIMERS; i++)
        tcb.msTimers[i] = msTick;
    for (i = 0; i < NUM_CS_TIMERS; i++)
        tcb.csTimers[i] = csTick;
}

// TIMER R/W //

unsigned long long tmTimerRead(const unsigned long long int *timerArray,
                               uint8_t timerIndex,
                               uint8_t maxTimers,
                               unsigned long long (*getNow)(void))
{
    if (timerIndex >= maxTimers)
        return 0ULL;

    unsigned long long result = 0ULL;

    pthread_mutex_lock(&tcb.mutex);

    if (tcb.initialized)
    {
        // this retrieves a time delta between the stored timestamp and the current one
        // timestamps always represent a point in the past
        result = getNow() - timerArray[timerIndex];
    }

    pthread_mutex_unlock(&tcb.mutex);
    return result;
}

void tmTimerWrite(unsigned long long *timerArray,
                  uint8_t timerIndex,
                  uint8_t maxTimers,
                  unsigned long long value,
                  unsigned long long (*getNow)(void))
{
    if (timerIndex >= maxTimers)
        return;

    pthread_mutex_lock(&tcb.mutex);

    if (tcb.initialized)
    {
        // ( - value ) stores an older timestamp
        // timestamps always represent a point in the past
        timerArray[timerIndex] = getNow() - value;
    }

    pthread_mutex_unlock(&tcb.mutex);
}

// TIMER SLOTS //

unsigned long Timer(uint8_t Timer)
{
    // slow timer 10 ticks per second
    return FastTimer(Timer) / 10;
}

unsigned long FastTimer(uint8_t Timer)
{
    return tmTimerRead(tcb.csTimers, Timer, NUM_CS_TIMERS, TimerGetCS);
}

void SetTimer(uint8_t Timer, unsigned long Value)
{
    return tmTimerWrite(tcb.csTimers, Timer, NUM_CS_TIMERS, Value, TimerGetCS);
}

void ClearTimer(uint8_t Timer)
{
    return SetTimer(Timer, 0UL);
}

unsigned long TimerMS(uint8_t Timer)
{
    return tmTimerRead(tcb.msTimers, Timer, NUM_MS_TIMERS, TimerGetMS);
}

void SetTimerMS(uint8_t Timer, unsigned long Value)
{
    return tmTimerWrite(tcb.msTimers, Timer, NUM_MS_TIMERS, Value, TimerGetMS);
}

void ClearTimerMS(uint8_t Timer)
{
    return SetTimerMS(Timer, 0UL);
}

unsigned long TimerUS(uint8_t Timer)
{
    return tmTimerRead(tcb.usTimers, Timer, NUM_US_TIMERS, TimerGetUS);
}

void SetTimerUS(uint8_t Timer, unsigned long Value)
{
    return tmTimerWrite(tcb.usTimers, Timer, NUM_US_TIMERS, Value, TimerGetUS);
}

void ClearTimerUS(uint8_t Timer)
{
    return SetTimerUS(Timer, 0UL);
}

// WAITS //

unsigned long TimerWait(unsigned long Time)
{
    return TimerGetMS() + Time;
}

void TimerReady(unsigned long Timer)
{
    // wait until the timer has elapsed
    long deltaTime = Timer - TimerGetMS();
    if (deltaTime > 0)
        Wait(deltaTime);
}

// CURRENT TIME QUERIES //

unsigned long long TimerGetUS(void)
{
    struct timeval tv = {0};
    // this is documented not to fail
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

unsigned long long TimerGetMS(void)
{
    struct timeval tv = {0};
    // this is documented not to fail
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

unsigned long long TimerGetCS(void)
{
    return TimerGetMS() / 10ULL;
}
