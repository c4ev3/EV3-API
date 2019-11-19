/** \file ev3_button.h
 * \brief Functions and constants for controlling EV3 buttons and LEDs
 *
 * ev3_button.h contains declarations for the EV3 C API button and LED functions.
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
 * \date 2013-06-20
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2016-04-19
 * \version 2
 * \note Correction of function name [void ButtonWaitForPressAndRelease(byte Button)]
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EV3_BUTTON_PRIVATE_H
#define EV3_BUTTON_PRIVATE_H

#include "../include/ev3_button.h"

bool ButtonLedInit();

bool ButtonLedOpen();

bool ButtonLedClose();

bool ButtonLedExit();

bool ButtonLedInitialized();

#endif // ev3_button_h

#ifdef __cplusplus
}
#endif
