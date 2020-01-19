/** \file ev3_timer.c
 * \brief Implementation of periodic timers of the ev3_timer module
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

#include "ev3.h"
#include "ev3_timer.private.h"

////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/*!
 * \brief Initialize all callback arrays to an empty state.
 */
static
bool tmInitArrays(void);

/*!
 * \brief Initialize kernel epoll, timerfd & eventfd objects.
 * \return True if it succeeded, false otherwise.
 */
static
bool tmInitKernel(void);

/*!
 * \brief Initialize and launch the waiter/callback caller thread.
 *
 * Also initializes the mutex used by timestamp functions.
 *
 * \return True if the operation succeeded, false otherwise.
 */
static
bool tmInitThread(void);

/*!
 * \brief Entrypoint of the waiter/callback caller thread.
 * \return not used
 */
static
void *tmThreadEntry(void *);

/*!
 * \brief Wait for a timer or an interruption event.
 *
 * This will wait on the epoll-fd until an event
 * comes from either a timer-fd (timer expiration) or an event-fd (termination).
 *
 * \param pEvent Where to put the event.
 * \return True if an event was successfully received; false otherwise.
 */
static
bool tmThreadWaitForEvent(struct epoll_event *pEvent);

/*!
 * \brief Process an event received from epoll().
 * \param pEvent Metadata of one epoll event (i.e. FD tag)
 * \param pLocalCopy Thread-safe location for a shadow copy of a callback block.
 * \return Whether event processing should continue (true) or not (false).
 */
static
bool tmThreadProcessEvent(struct epoll_event *pEvent, DynamicCArray *pLocalCopy);

/*!
 * \brief Destroy the kernel epoll, timerfd and eventfd objects.
 * \return True if it succeeded, false otherwise.
 */
static
bool tmExitKernel(void);

/*!
 * \brief Attempt to close the given FD.
 * \param pFd File descriptor to close and set to -1.
 * \param failMessage Message to print if the close fails.
 * \return Whether the close succeeeded (true) or not (false)
 */
static
bool tmTryClose(int *pFd, const char *failMessage);

/*!
* \brief Deallocate all callback arrays.
*/
static
void tmFreeArrays(void);

/*!
 * \brief Prepare the kernel timer for servicing a callback at a given rate.
 * \param interval Time interval between two calls to callback.
 * \return Whether the timer was enabled successfully.
 */
static
bool tmEnablePeriod(TimerInterval interval);

/*!
 * \brief Shut down a given kernel timer.
 * \param interval Time interval between two calls to callback.
 * \return Whether the timer was disabled successfully.
 */
static
bool tmDisablePeriod(TimerInterval interval);

/*!
 * \brief Stop and destroy the waiter/callback caller thread.
 *
 * Also destroys the mutex used by the timestamp API functions.
 *
 * \return True if the operation succeeded, false otherwise.
 */
static
bool tmExitThread(void);

/*!
 * Get number of milliseconds that a given interval takes.
 * \param interval Interval to identify.
 * \return Its duration in milliseconds.
 */
static
int tmGetIntervalMilliseconds(TimerInterval interval);

////////////////////////////////////////////////////////////////
// STATIC DATA

//! Global data of the timer module.
struct TimerControlBlock tcb = {
        .initialized = false,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
};


////////////////////////////////////////////////////////////////
// IMPLEMENTATION

// INITIALIZATION

bool TimerInitialized(void)
{
    pthread_mutex_lock(&tcb.mutex);
    bool value = tcb.initialized;
    pthread_mutex_unlock(&tcb.mutex);
    return value;
}

bool TimerInit(void)
{
    pthread_mutex_lock(&tcb.mutex);

    if (tcb.initialized)
        goto success;

    // initialize NXC-style timestamp/timer slots
    tmInitTimestampSlots();

    // initialize callback blocks to an empty state
    if (!tmInitArrays())
        goto failure;

    // initialize kernel objects (epollfd, eventfd, timerfd)
    if (!tmInitKernel())
        goto failure;

    // initialize & start callback thread
    if (!tmInitThread())
        goto failure;

success:
    tcb.initialized = true;
    pthread_mutex_unlock(&tcb.mutex);
    return true;

failure:
    tcb.initialized = true; // to pass TimerExit() check
    pthread_mutex_unlock(&tcb.mutex);
    TimerExit();
    return false;
}

bool tmInitArrays(void)
{
    for (TimerInterval interval = ti10ms; interval < ti_count; interval++)
    {
        // preallocate all callback arrays
        if (!CArrayInit(&tcb.callbacksFor[interval].functions,
                        sizeof(TimerCallback), __alignof__(TimerCallback),
                        INITIAL_CALLBACKS))
            return false;
    }
    return true;
}

// workaround for old libc
#ifndef TFD_CLOEXEC
#define TFD_CLOEXEC O_CLOEXEC
#endif
#ifndef TFD_NONBLOCK
#define TFD_NONBLOCK O_NONBLOCK
#endif

bool tmInitKernel(void)
{
    int retval = 0;
    struct epoll_event event = {
            .events = EPOLLIN,
            .data.u32 = 0
    };

    // initialize epoll
    tcb.epollFD = epoll_create(1);
    if (tcb.epollFD < 0)
    {
        perror("epoll_create1() failed");
        return false;
    }

    for (TimerInterval i = ti10ms; i < ti_count; i++)
    {
        // create a timer object
        int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
        if (tfd < 0)
        {
            perror("timerfd_create() failed");
            return false;
        }

        // set shared memory values
        tcb.callbacksFor[i].timerFD = tfd;
        tcb.callbacksFor[i].running = false;

        // and register it in the epoll object
        event.data.u32 = i;
        retval = epoll_ctl(tcb.epollFD, EPOLL_CTL_ADD, tfd, &event);
        if (retval < 0)
        {
            perror("epoll_ctl(EPOLL_CTL_ADD) (timerfd registration) failed");
            return false;
        }
    }

    // initialize terminating eventfd
    tcb.stopFD = eventfd(0, 0);
    if (tcb.stopFD < 0)
    {
        perror("eventfd() failed");
        return false;
    }

    // and register it as well
    event.data.u32 = TERMINATOR;
    retval = epoll_ctl(tcb.epollFD, EPOLL_CTL_ADD, tcb.stopFD, &event);
    if (retval < 0)
    {
        perror("epoll_ctl(EPOLL_CTL_ADD) (eventfd registration) failed");
        return false;
    }
    return true;
}

bool tmInitThread(void)
{
    // start the thread
    int retval = pthread_create(&tcb.waiterThread, NULL, tmThreadEntry, NULL);
    if (retval < 0)
    {
        perror("pthread_create() failed");
        return false;
    }

    return true;
}


// THREAD LOOP //

void *tmThreadEntry(void *arg)
{
    (void) arg;

    // initialize local shadow copy
    // (used while calling handlers to prevent modification while iterating)
    DynamicCArray localCopy = DYNAMIC_CARRAY_INITIALIZER_FOR(TimerCallback);

    struct epoll_event event = {0};
    // and loop until error (or termination)
    while (tmThreadWaitForEvent(&event))
    {
        // and handle it
        if (!tmThreadProcessEvent(&event, &localCopy))
            break;
    }

    // clean up the shadow array
    CArrayFree(&localCopy);
    return NULL;
}

bool tmThreadWaitForEvent(struct epoll_event *pEvent)
{
    while (true)
    {
        // let's try to wait
        int result = epoll_wait(tcb.epollFD, pEvent, 1, -1);
        if (result > 0) // event read successfully
            return true;

        if (errno == EINTR)
            continue;

        perror("epoll_wait() failed");
        return false;
    }
}

bool tmThreadProcessEvent(struct epoll_event *pEvent, DynamicCArray *pLocalCopy)
{
    // the termination eventfd is tagged with TERMINATOR
    // so when we receive an event from it, just quit the loop
    if (pEvent->data.u32 == TERMINATOR)
        return false;

    // otherwise the tag is the interval enum value
    TimerInterval interval = pEvent->data.u32;

    // critical section

    pthread_mutex_lock(&tcb.mutex);
    // mirror the corresponding callback block (this avoids the need for recursive mutexes)
    int timerFD = tcb.callbacksFor[interval].timerFD;
    bool allocOk = CArrayMirror(pLocalCopy, &tcb.callbacksFor[interval].functions);
    // leave critical section
    pthread_mutex_unlock(&tcb.mutex);

    if (!allocOk)
        return false;

    // process the pending read (so that the event does not trigger again)
    // also get the elapsed expiration count to loop multiple times, if needed
    // the read itself is nonblocking, see EAGAIN branch below
    uint64_t elapsedExpirations = 0;
    int retval = read(timerFD, &elapsedExpirations, sizeof(elapsedExpirations));

    if (retval < 0)
    {
        if (errno == EAGAIN)
        {
            // this may happen if the timer gets disarmed between
            // the epoll_wait() return and the read() start
            elapsedExpirations = 0;
        }
        else
        {
            perror("read(timerfd) failed");
            return false;
        }
    }

    // and finally, call the function the needed number of times
    DYNAMIC_CARRAY_FOREACH(ptrToFnPtr, pLocalCopy)
    {
        TimerCallback fn = * (TimerCallback*) ptrToFnPtr;
        for (uint64_t n = 0; n < elapsedExpirations; n++)
            fn(interval);
    }

    return true;
}

// DESTRUCTION //

bool TimerExit(void)
{
    bool succeeded = true;

    pthread_mutex_lock(&tcb.mutex);
    if (!tcb.initialized)
        goto exit;
    tcb.initialized = false;

    // stop the waiter thread and wait for its exit
    if (!tmExitThread())
        succeeded = false;

    // clean up kernel objects
    if (!tmExitKernel())
        succeeded = false;

    // free callback array allocations
    tmFreeArrays();

exit:
    pthread_mutex_unlock(&tcb.mutex);
    return succeeded;
}

bool tmExitThread(void)
{
    bool success = true;
    int retval = 0;

    // notify and join only if we are not the cleanup thread
    // - otherwise pthread_join() triggers UB
    bool weAreTimerThread = tcb.waiterThread == pthread_self();
    if (!weAreTimerThread)
    {
        // notify the thread that it should quit
        retval = eventfd_write(tcb.stopFD, 1);
        if (retval < 0)
        {
            perror("eventfd_write() failed");
            success = false;
        }

        // wait for the thread to terminate
        pthread_mutex_unlock(&tcb.mutex); // prevent deadlock
        retval = pthread_join(tcb.waiterThread, NULL);
        pthread_mutex_lock(&tcb.mutex);
        if (retval < 0)
        {
            perror("pthread_join() failed");
            success = false;
        }
    }

    return success;
}

bool tmExitKernel(void)
{
    bool success = true;

    for (TimerInterval i = ti10ms; i < ti_count; i++)
    {
        if (!tmTryClose(&tcb.callbacksFor[i].timerFD, "close(timerfd) failed"))
            success = false;
    }

    // close the eventfd terminator
    if (!tmTryClose(&tcb.stopFD, "close(eventfd) failed"))
        success = false;

    // and close the epoll waiter as well
    if (!tmTryClose(&tcb.epollFD, "close(epollfd) failed"))
        success = false;

    return success;
}

bool tmTryClose(int *pFd, const char *failMessage)
{
    bool success = true;
    if (*pFd >= 0)
    {
        int retval = close(*pFd);
        if (retval < 0)
        {
            perror(failMessage);
            success = false;
        }
        *pFd = -1;
    }
    return success;
}

void tmFreeArrays(void)
{
    for (TimerInterval interval = ti10ms; interval < ti_count; interval++)
        CArrayFree(&tcb.callbacksFor[interval].functions);
}

// ADD/REMOVE CALLBACKS //

bool SetTimerCallback(TimerInterval interval, TimerCallback callback)
{
    // prevent out-of-bounds access
    if (interval < ti10ms || interval >= ti_count)
        return false;

    // rudimentary check against passing an invalid function
    if (callback == NULL)
        return false;

    bool success = false;

    // critical operation, lock the mutex
    pthread_mutex_lock(&tcb.mutex);

    if (!tcb.initialized)
        goto exit;

    // add the callback to the array
    TimerCallback *pNew = CArrayStackPush(&tcb.callbacksFor[interval].functions);

    if (pNew)
    {
        *pNew = callback;
        // ensure that the callback will fire
        success = tmEnablePeriod(interval);
    }

exit:
    // unlock mutex & return success state
    pthread_mutex_unlock(&tcb.mutex);
    return success;
}

bool RemoveTimerCallback(TimerInterval interval, TimerCallback callback)
{
    // prevent out-of-bounds access
    if (interval < ti10ms || interval >= ti_count)
        return false;

    // rudimentary check against passing an invalid function
    if (callback == NULL)
        return false;

    bool success = false;

    // critical operation, lock the mutex
    pthread_mutex_lock(&tcb.mutex);

    if (!tcb.initialized)
        goto exit;

    // remove the callback from the array
    CallbackBlock *pBlock = &tcb.callbacksFor[interval];

    for (size_t i = 0; i < pBlock->functions.count; i++)
    {
        TimerCallback *ptr = CArrayAccess(&pBlock->functions, i);
        if (*ptr == callback)
        {
            success = CArrayRemove(&pBlock->functions, i, 1);
            break;
        }
    }

    // disable the callback period if no handlers are using it
    if (success && pBlock->functions.count == 0)
        success = tmDisablePeriod(interval);

exit:
    // unlock mutex & return success state
    pthread_mutex_unlock(&tcb.mutex);
    return success;
}

// ENABLE & DISABLE TIMERS

bool tmEnablePeriod(TimerInterval interval)
{
    // check if we are not done already
    if (tcb.callbacksFor[interval].running)
        return true;

    // otherwise fill the timer parameter struct (arms the timer)
    int millis = tmGetIntervalMilliseconds(interval);
    int seconds = millis / 1000;
    int nanos = (millis % 1000) * 1000000;

    struct itimerspec params = {
            .it_value = {.tv_sec = seconds, .tv_nsec = nanos},
            .it_interval = {.tv_sec = seconds, .tv_nsec = nanos}
    };

    // push the result to kernel

    int retval = timerfd_settime(tcb.callbacksFor[interval].timerFD, 0, &params, NULL);
    if (retval < 0)
    {
        perror("timerfd_settime() (timer startup) failed");
        return false;
    }

    tcb.callbacksFor[interval].running = true;
    return true;
}

bool tmDisablePeriod(TimerInterval interval)
{
    // check if we are not done already
    if (!tcb.callbacksFor[interval].running)
        return true;

    // otherwise set all to zero (disarms timer)
    struct itimerspec params = {0};

    // push the result to kernel
    int retval = timerfd_settime(tcb.callbacksFor[interval].timerFD, 0, &params, NULL);
    if (retval < 0)
    {
        perror("timerfd_settime() (timer shutdown) failed");
        return false;
    }

    tcb.callbacksFor[interval].running = false;
    return true;
}

int tmGetIntervalMilliseconds(TimerInterval interval)
{
    // just translate from ordered numbers to actual intervals
    switch (interval)
    {
        case ti10ms:
            return 10;
        case ti50ms:
            return 50;
        case ti100ms:
            return 100;
        case ti250ms:
            return 250;
        case ti500ms:
            return 500;
        default:
        case ti1sec:
            return 1000;
    }
}
