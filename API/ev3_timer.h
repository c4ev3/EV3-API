/** \file ev3_timer.h
 * \brief Functions and constants for controlling EV3 timers
 *
 * ev3_timer.h contains declarations for the EV3 C API timer functions.
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
 * \date 2013-07-10
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek (linuxtardis_at_gmail.com)
 * \date 2020-02-13
 * \version 2
 * \note added docs, initialization functions and RemoveTimerCallback()
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_timer_h
#define ev3_timer_h

#include "ev3_constants.h"
#include <stdbool.h>

/*!
 * \brief Available asynchronous timer intervals.
 * \sa TimerCallback, SetTimerCallback(), RemoveTimerCallback()
 */
typedef enum
{
    //! Interval of 10 milliseconds
    ti10ms,
    //! Interval of 50 milliseconds
    ti50ms,
    //! Interval of 100 milliseconds
    ti100ms,
    //! Interval of 250 milliseconds
    ti250ms,
    //! Interval of 500 milliseconds
    ti500ms,
    //! Interval of 1 second
    ti1sec,
    //! number of timer intervals; do not use as an interval
    ti_count
} TimerInterval;

//! Callback for asynchronous timer.
//! The interval argument will receive the interval type (TimerInterval) that triggered this event.
typedef void (*TimerCallback)(int interval);

/*!
 * \brief Initialize the timer module.
 * \remark This function is not thread safe.
 * \return True if the operation succeeded, false otherwise.
 */
bool TimerInit(void);

/*!
 * \brief Deinitialize the timer module.
 * \remark This function is not thread safe.
 * \return True if the operation succeeded, false otherwise.
 */
bool TimerExit(void);

/*!
 * \brief Check whether the timer module is initialized.
 * \remark This function is thread safe.
 * \return True if yes, false otherwise.
 */
bool TimerInitialized(void);


/*!
 * \brief Register a new asynchronous callback timer.
 * \param interval Time period between calls to callback.
 * \param callback Callback to call.
 * \remark This function is thread safe (however not against TimerInit() and TimerExit() functions).
 * \return True if the operation succeeded, false otherwise.
 */
bool SetTimerCallback(TimerInterval interval, TimerCallback callback);

/*!
 * \brief Deregister an existing asynchronous callback timer.
 * \param interval Time period between calls to callback.
 * \param callback Callback to remove.
 * \remark This function is thread safe (however not against TimerInit() and TimerExit() functions).
 * \return True if the operation succeeded, false otherwise.
 */
bool RemoveTimerCallback(TimerInterval interval, TimerCallback callback);


/*!
 * \brief Get the global time advanced by a given interval.
 * \param Time Time interval to add.
 * \remark This function is thread safe.
 * \return Timestamp value that the global millisecond timer will have in the future.
 */
unsigned long TimerWait(unsigned long Time);

/*!
 * \brief Wait the given global time.
 * \param Timer Global millisecond timer value to wait for.
 * \remark This function is thread safe.
 */
void TimerReady(unsigned long Timer);

/*!
 * \brief Query a CS timer (in deciseconds)-
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function)
 * \return Deciseconds elapsed since the timer's origin.
 */
unsigned long Timer(uint8_t Timer);

/*!
 * \brief Query a CS timer (in centiseconds).
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function)
 * \return Centiseconds elapsed since the timer's origin.
 */
unsigned long FastTimer(uint8_t Timer);

/*!
 * \brief Reset origin of a CS timer to the current time.
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void ClearTimer(uint8_t Timer);

/*!
 * \brief Set origin of a CS timer.
 * \param Timer Timer slot index.
 * \param Value Value that the timer would now have.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void SetTimer(uint8_t Timer, unsigned long Value);

/*!
 * \brief Query a MS timer.
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function).
 * \return Milliseconds elapsed since the timer's origin.
 */
unsigned long TimerMS(uint8_t Timer);

/*!
 * \brief Reset origin of a MS timer to the current time.
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void ClearTimerMS(uint8_t Timer);

/*!
 * \brief Set origin of a MS timer.
 * \param Timer Timer slot index.
 * \param Value Value that the timer would now have.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void SetTimerMS(uint8_t Timer, unsigned long Value);

/*!
 * \brief Query a US timer.
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function).
 * \return Microseconds elapsed since the timer's origin.
 */
unsigned long TimerUS(uint8_t Timer);

/*!
 * \brief Reset origin of a US timer to the current time.
 * \param Timer Timer slot index.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void ClearTimerUS(uint8_t Timer);

/*!
 * \brief Set origin of a US timer.
 * \param Timer Timer slot index.
 * \param Value Value that the timer would now have.
 * \remark This function is thread safe (however not against TimerExit() function).
 */
void SetTimerUS(uint8_t Timer, unsigned long Value);

/*!
 * \brief Query global time in microseconds.
 * \remark This function is thread safe.
 * \return Timestamp value that the global microsecond timer currently has.
 */
unsigned long long TimerGetUS(void);

/*!
 * \brief Query global time in milliseconds.
 * \remark This function is thread safe.
 * \return Timestamp value that the global millisecond timer currently has.
 */
unsigned long long TimerGetMS(void);

/*!
 * \brief Query global time in centiseconds.
 * \remark This function is thread safe.
 * \return Timestamp value that the global centisecond timer currently has.
 */
unsigned long long TimerGetCS(void);

#ifdef __cplusplus
}
#endif
#endif
