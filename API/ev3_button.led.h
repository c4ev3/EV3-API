/** \file ev3_button.led.h
 * \brief Public API for controlling brick LEDs.
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

#ifndef ev3_button_led_h
#define ev3_button_led_h

#include "ev3_constants.h"
#include <stdbool.h>

/*!
 * \brief Initialize the LED interface.
 * \return Whether the initialization succeeded or not.
 */
bool LedInit(void);

/*!
 * \brief Deinitialize the LED interface.
 * \return Whether the deinitialization succeeded or not.
 */
bool LedExit(void);

/*!
 * \brief Check whether the LED interface is initialized.
 * \return Whether the module is initialized or not.
 */
bool LedInitialized(void);

/*!
 * \brief Check whether a warning mode (LEDs always orange) is currently activated.
 * \return True if a warning light is activated, false otherwise.
 */
bool LedWarning();

/*!
 * \brief Set new warning mode state (LEDs always orange).
 * \param Value True to activate always-orange LEDs, false otherwise.
 */
bool SetLedWarning(bool Value);

/*!
 * \brief Check the current LED pattern (color & blink mode).
 * \return Current LED pattern value.
 * \sa LEDPattern
 */
LEDPattern LedPattern();

/*!
 * \brief Set new LED pattern (color & blink mode).
 * \param newPattern New pattern to set.
 * \sa LEDPattern
 */
bool SetLedPattern(LEDPattern newPattern);

#endif // ev3_button_led_h

#ifdef __cplusplus
}
#endif
