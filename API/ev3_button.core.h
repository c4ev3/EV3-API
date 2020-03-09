/** \file ev3_button.core.h
 * \brief Low level abstractions for driving EV3 buttons & LEDs.
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
 * \author Jakub Vanek (linuxtardis at gmail.com)
 * \date 2020-01-22
 * \version 1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_button_core_h
#define ev3_button_core_h

////////////////////////////////////////////////////////////////////////////////
// INCLUDES

#include "ev3_constants.h"
#include <stdbool.h>

/*!
 * \brief Initialize the button kernel interface.
 * \return Whether the initialization succeeded or not.
 */
bool ButtonCoreInit(void);

/*!
 * \brief Deinitialize the button kernel interface.
 * \return Whether the deinitialization succeeded or not.
 */
bool ButtonCoreExit(void);

/*!
 * \brief Check if the kernel button interface is initialized.
 * \return Whether the module is initialized or not.
 */
bool ButtonCoreInitialized(void);

/*!
 * \brief Read the brick hardware revision from the kernel.
 * \return String received from the kernel.
 */
const char *ButtonCoreReadHWRev(void);

/*!
 * \brief Write the specified LED pattern to kernel.
 * \param pattern Pattern to set.
 * \return Whether the operation succeeded or not.
 */
bool ButtonCoreSetLEDs(LEDPattern pattern);

/*!
 * \brief Read button states from the kernel.
 * \param buttonStates Where to save the press states.
 *                     This is an array containing 6 bytes, each having either value 0 (b. released) or 1 (b. pressed).
 *                     The button indices correspond to BUTTON_IDX_* constants.
 * \return Whether the operation succeeded or not.
 */
bool ButtonCoreReadButtons(uint8_t *buttonStates);

/*!
 * \brief Read a state of one button from the kernel.
 * \param button Button to check.
 * \return 1 if the button is pressed, 0 if not, -1 if an error occured.
 */
int ButtonCoreReadButton(Button button);

#endif // ev3_button_core_h

#ifdef __cplusplus
}
#endif
