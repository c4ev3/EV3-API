/** \file ev3_timer.private.h
 * \brief Private declarations of the ev3_timer module
 *
 * This file contains implementation-internal declarations
 * that are not safe to use by user programs. For that purpose,
 * please see the ev3_timer.h header.
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
 * The Initial Developer of this code is Jakub Vanek.
 * Portions created by Jakub Vanek are Copyright (C) 2020 Jakub Vanek.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek (linuxtardis_at_gmail.com)
 * \date 2020-02-11
 * \version 1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_timer_private_h
#define ev3_timer_private_h


////////////////////////////////////////////////////////////////
// INCLUDES

#include "ev3_timer.h"
#include "ev3_array.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

////////////////////////////////////////////////////////////////
// MACROS

//! initial capacity of each callback queue
#define INITIAL_CALLBACKS  4
#define TERMINATOR UINT32_MAX

////////////////////////////////////////////////////////////////
// GLOBAL DATA

//! Unified array of multiple callbacks for one time interval.
typedef struct
{
    //! Array of functions.
    DynamicCArray functions;
    //! Kernel-provided timer object that we can call epoll() on.
    int timerFD;
    //! Whether the timer is currently enabled.
    bool running;
} CallbackBlock;

//! Structure for the timer module data.
struct TimerControlBlock
{
    //! Millisecond timer slots.
    unsigned long long msTimers[NUM_MS_TIMERS];
    //! Microsecond timer slots.
    unsigned long long usTimers[NUM_US_TIMERS];
    //! Centisecond timer slots.
    unsigned long long csTimers[NUM_CS_TIMERS];

    //! Periodic callbacks
    CallbackBlock callbacksFor[ti_count];

    //! Waiter/callback caller thread; statically initialized
    pthread_t waiterThread;
    //! Shared mutex, statically initialized
    pthread_mutex_t mutex;
    //! Waiter epoll fd
    int epollFD;
    //! Eventfd for terminating the looping thread
    int stopFD;

    //! Stores whether the module is currently initialized.
    bool initialized;
};

//! Global data of the timer module.
extern struct TimerControlBlock tcb;


////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS SHARED BETWEEN MODULES

/*!
 * \brief Initialize timer slot arrays.
 */
void tmInitTimestampSlots(void);


#ifdef __cplusplus
}
#endif
#endif
