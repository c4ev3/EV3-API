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

#include "ev3_time.h"
#include <time.h>

static void ms2ts(struct timespec *ts, unsigned long ms)
{
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000;
}

void Wait(unsigned long ms)
{
    // TODO: Can we avoid the busy wait and use usleep?
	struct timespec delay;
	ms2ts(&delay, ms);
	while (nanosleep(&delay, &delay));
}


unsigned long long TimerGetUS() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (((unsigned long long)tv.tv_sec) * 1000000) + tv.tv_usec;
}

unsigned long long TimerGetMS() {
    return TimerGetUS() / 1000;
}
