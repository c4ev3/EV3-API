/*
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
 * Portions  by John Hansen are Copyright (C) 2009-2013 John Hansen.
 * All Rights Reserved.
 */

#include "ev3_command.h"
#include <time.h>

static void ms2ts(struct timespec *ts, unsigned long ms)
{
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000;
}

void Wait(unsigned long ms)
{
	struct timespec delay;
	ms2ts(&delay, ms);
	while (nanosleep(&delay, &delay));
}

