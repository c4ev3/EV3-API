/** \file ev3_button.h
 * \brief Functions and constants for controlling EV3 buttons and LEDs
 *
 * ev3_button.h contains declarations for the EV3 C API button and LED functions.
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
 * \date 2013-06-20
 * \version 1
 *
 * ----------------------------------------------------------------------------
 *
 * \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2016-04-19
 * \version 2
 * \note Correction of function name [void ButtonWaitForPressAndRelease(byte Button)]
 *
 * ----------------------------------------------------------------------------
 *
 * \author Jakub Vanek (linuxtardis at gmail.com)
 * \date 2020-01-22
 * \version 3
 * \note Add docs & implement NXC API
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_button_h
#define ev3_button_h

#include "ev3_constants.h"
#include "ev3_button.core.h"
#include "ev3_button.led.h"
#include "ev3_button.lejos.h"
#include "ev3_button.nxc.h"

/*!
 * \brief Initialize the Button/LED EV3 module
 * \return True if initialization succeeded, false otherwise.
 */
bool ButtonLedInit();

/*!
 * \brief Deinitialize the Button/LED EV3 module
 * \return True if deinitialization succeeded, false otherwise.
 */
bool ButtonLedExit();

/*!
 * \brief Check whether the Button/LED module is initialized.
 * \return True if it is ready, false otherwise.
 */
bool ButtonLedInitialized();

/*!
 * \brief Query the brick hardware revision (float version).
 * \return Hardware revision as a floating point number.
 */
float HardwareVersion();

/*!
 * \brief Query the brick hardware revision (string version).
 * \return Hardware revision in the form of a "0.00" string.
 */
const char *HardwareVersionString();

#endif // ev3_button_h

#ifdef __cplusplus
}
#endif
