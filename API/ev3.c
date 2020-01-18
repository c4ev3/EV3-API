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
 * The Initial Developer of this code is Simón Rodriguez Perez.
 * Portions created by Simón Rodriguez Perez are Copyright (C) 2014-2015 Simón Rodriguez Perez.
 * All Rights Reserved.
 *
 */

#include "ev3.h"

static bool initialized;
int __attribute__((constructor)) InitEV3 (void)
{
	if (EV3IsInitialized())
	    return 1;

	OutputInit();
	SensorsInit();
	ButtonLedInit();
	LcdInit();
	SoundInit();
    
	LcdClean();

	initialized = true;
	return 1;
}

int __attribute__((destructor)) FreeEV3()

{
	OutputExit();
	SensorsExit();
	ButtonLedExit();
	LcdExit();
	SoundExit();
	initialized = false;

	return 1;
}

bool EV3IsInitialized(void)
{
	return initialized;
}
