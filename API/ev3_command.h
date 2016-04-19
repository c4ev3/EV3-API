/** \file ev3_command.h
 * \brief Functions and constants for controlling misc EV3 items
 *
 * ev3_command.h contains declarations for the EV3 C API command functions.
 *
 * License:
 *
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
