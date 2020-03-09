/** \file ev3_button.c
 * \brief Implementation of the button module
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
 * Portions  by Jakub Vanek are Copyright (C) 2020 Jakub Vanek.
 * All Rights Reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek
 * \date 2020-01-22
 * \version 1
 */

#include "ev3.h"

////////////////////////////////////////////////////////////////////////////////
// LIFETIME

bool ButtonLedInitialized()
{
    return ButtonCoreInitialized() &&
           LedInitialized() &&
           ButtonNxcInitialized();
}

bool ButtonLedInit()
{
    return ButtonCoreInit() &&
           LedInit() &&
           ButtonNxcInit();
}

bool ButtonLedExit()
{
    return ButtonNxcExit() &
           LedExit() &
           ButtonCoreExit();
}

////////////////////////////////////////////////////////////////////////////////
// HARDWARE REVISION

float HardwareVersion()
{
    // string begins with letter 'V'; on error 'V0.00' is returned (which is OK)
    const char *floatStart = HardwareVersionString() + 1;

    // if conversion fails, 0.0f is returned (which is OK)
    return strtof(floatStart, NULL);
}

const char *HardwareVersionString()
{
    const char *str = ButtonCoreReadHWRev();

    return str == NULL ? "V0.00" : str;
}
