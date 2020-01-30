/** \file ev3_command.h
 * \brief Functions and constants for controlling misc EV3 items
 *
 * ev3_command.h contains declarations for the EV3 C API command functions.
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
 * Portions  by John Hansen are Copyright (C) 2009-2013 John Hansen.
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

#ifndef ev3_command_h
#define ev3_command_h

#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#include "ev3_constants.h"

void Wait(unsigned long ms);

#endif // ev3_command_h

#ifdef __cplusplus
}
#endif
