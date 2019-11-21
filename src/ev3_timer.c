/*
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
 */

#include "../include/ev3_timer.h"

#ifndef DISABLE_TIMERS

static unsigned long long msTimers[NUM_MS_TIMERS];
static unsigned long long usTimers[NUM_US_TIMERS];

#define TIMER_INTERVAL 10000 // 10 ms
#define TIMER_10MS     1
#define TIMER_50MS     5
#define TIMER_100MS    10
#define TIMER_250MS    25
#define TIMER_500MS    50
#define TIMER_1SEC     100
#define MAX_CALLBACKS  4

static TimerCallback callBack10ms[MAX_CALLBACKS];
static TimerCallback callBack50ms[MAX_CALLBACKS];
static TimerCallback callBack100ms[MAX_CALLBACKS];
static TimerCallback callBack250ms[MAX_CALLBACKS];
static TimerCallback callBack500ms[MAX_CALLBACKS];
static TimerCallback callBack1s[MAX_CALLBACKS];

static int callBack10ms_count;
static int callBack50ms_count;
static int callBack100ms_count;
static int callBack250ms_count;
static int callBack500ms_count;
static int callBack1s_count;

static unsigned long timer_interval_mutliplier = 1;

unsigned long TimerWait(unsigned long Time) {
    return TimerGetMS() + Time;
}

void TimerReady(unsigned long Timer) {
    // wait until timer has elapsed
    // do not block other threads
    while (Timer > TimerGetMS()) {
        usleep(1000);
    }
}


unsigned long TimerMS(uint8_t timerId) {
    if (timerId < NUM_MS_TIMERS)
        return TimerGetMS() - msTimers[timerId];
    return 0;
}

void ClearTimerMS(uint8_t timerId) {
    if (timerId < NUM_MS_TIMERS)
        msTimers[timerId] = TimerGetMS();
}


unsigned long TimerUS(uint8_t timerId) {
    if (timerId < NUM_US_TIMERS)
        return TimerGetUS() - usTimers[timerId];
    return 0;
}

void ClearTimerUS(uint8_t timerId) {
    if (timerId < NUM_US_TIMERS)
        usTimers[timerId] = TimerGetUS();
}


void _timerSigHandler(int sig) {
    int index = 0;
    static unsigned long counter = 0;

    // Handle the 10ms ones first
    if (counter % TIMER_10MS == 0 || timer_interval_mutliplier <= TIMER_10MS) {
        for (index = 0; index < callBack10ms_count; index++) {
            callBack10ms[index](sig);
        }
    }
    // Handle the 50ms ones
    if (counter % TIMER_50MS == 0 || timer_interval_mutliplier <= TIMER_50MS) {
        for (index = 0; index < callBack50ms_count; index++) {
            callBack50ms[index](sig);
        }
    }

    // Handle the 100ms ones
    if (counter % TIMER_100MS == 0 || timer_interval_mutliplier <= TIMER_100MS) {
        for (index = 0; index < callBack100ms_count; index++) {
            callBack100ms[index](sig);
        }
    }

    // Handle the 250ms ones
    if (counter % TIMER_250MS == 0 || timer_interval_mutliplier <= TIMER_250MS) {
        for (index = 0; index < callBack250ms_count; index++) {
            callBack250ms[index](sig);
        }
    }

    // Handle the 500ms ones
    if (counter % TIMER_500MS == 0 || timer_interval_mutliplier <= TIMER_500MS) {
        for (index = 0; index < callBack500ms_count; index++) {
            callBack500ms[index](sig);
        }
    }

    // Handle the 1s ones
    if (counter % TIMER_1SEC == 0 || timer_interval_mutliplier <= TIMER_1SEC) {
        for (index = 0; index < callBack1s_count; index++) {
            callBack1s[index](sig);
        }
    }
    counter += timer_interval_mutliplier;
}

void _timerCallbackInit() {
    struct sigaction sa;
    struct itimerval timer;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &_timerSigHandler;
    sigaction(SIGALRM, &sa, NULL);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = TIMER_INTERVAL * timer_interval_mutliplier;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = TIMER_INTERVAL * timer_interval_mutliplier;
    setitimer(ITIMER_REAL, &timer, NULL);

    // Reset all the counters
    callBack10ms_count = 0;
    callBack50ms_count = 0;
    callBack100ms_count = 0;
    callBack250ms_count = 0;
    callBack500ms_count = 0;
    callBack1s_count = 0;

    // Ensure all pointers are NULL
    memset(callBack10ms, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
    memset(callBack50ms, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
    memset(callBack100ms, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
    memset(callBack250ms, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
    memset(callBack500ms, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
    memset(callBack1s, 0, sizeof(TimerCallback) * MAX_CALLBACKS);
}


bool addTimerCallback(TimerCallback callbacks[], int* callbackCount, TimerCallback callback) {
    if (*callbackCount < MAX_CALLBACKS) {
        callbacks[*callbackCount] = callback;
        *callbackCount += 1;
        return true;
    }
    return false;
}

bool AddTimerCallback(TimerInterval interval, TimerCallback callback) {
    if (callback != NULL) {
        switch (interval) {
            case ti10ms:
                return addTimerCallback(callBack10ms, &callBack10ms_count, callback);
            case ti50ms:
                return addTimerCallback(callBack50ms, &callBack50ms_count, callback);
            case ti100ms:
                return addTimerCallback(callBack100ms, &callBack100ms_count, callback);
            case ti250ms:
                return addTimerCallback(callBack250ms, &callBack250ms_count, callback);
            case ti500ms:
                return addTimerCallback(callBack500ms, &callBack500ms_count, callback);
            case ti1sec:
                return addTimerCallback(callBack1s, &callBack1s_count, callback);
            default:
                return false;
        }
    }
    return false;
}

void TimerInit(unsigned long interval_multiplier) {
    timer_interval_mutliplier = interval_multiplier;
    unsigned long long usTick = TimerGetUS();
    unsigned long long msTick = usTick / 1000;
    int i;
    for (i = 0; i < NUM_US_TIMERS; i++) {
        usTimers[i] = usTick;
    }
    for (i = 0; i < NUM_MS_TIMERS; i++) {
        msTimers[i] = msTick;
    }
    // also initialize our callback timers
    _timerCallbackInit();
}

#endif

unsigned long long TimerGetUS() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (((unsigned long long) tv.tv_sec) * 1000000) + tv.tv_usec;
}

unsigned long long TimerGetMS() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (((unsigned long long) tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

unsigned long long TimerGetCS() {
    return TimerGetMS() / 10;
}


