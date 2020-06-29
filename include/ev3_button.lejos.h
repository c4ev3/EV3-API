/** \file ev3_button.lejos.h
 * \brief leJOS-like API for accessing brick buttons.
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

#ifndef ev3_button_lejos_h
#define ev3_button_lejos_h

#include "ev3_constants.h"
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// MACROS

//! shift bits in order to make leJOS released button bitmask
#define EVENT_MASK_MAKE_RELEASED(mask) ((unsigned int)(mask) << 8u)

//! shift bits in order to make leJOS pressed button bitmask
#define EVENT_MASK_MAKE_PRESSED(mask)  ((unsigned int)(mask) << 0u)

//! shift bits in order to make leJOS complete button bitmask
#define EVENT_MASK_MAKE(pressed, released) (EVENT_MASK_MAKE_PRESSED(pressed) | \
                                            EVENT_MASK_MAKE_RELEASED(released))

//! shift bits in order to get released button bitmask
#define EVENT_MASK_GET_RELEASED(mask) (((mask) >> 8u) & 0xFF)

//! shift bits in order to get pressed button bitmask
#define EVENT_MASK_GET_PRESSED(mask)  (((mask) >> 0u) & 0xFF)

//! check if a given bitmask contains
#define BUTTON_MASK_CONTAINS(mask, button)  ((BUTTON_MASK_OF(button) & (mask)) != 0)

//! generate bitmask of a button id
#define BUTTON_MASK_OF(button) (1u << (button))

////////////////////////////////////////////////////////////////////////////////
// PUBLIC DECLARATIONS

/*!
 * \brief Check whether the given button is currently released.
 * \param Button Button to check.
 * \return True if the button is released, false otherwise.
 */
bool ButtonIsUp(Button Button);

/*!
 * \brief Check whether the given button is currently pressed.
 * \param Button Button to check.
 * \return True if the button is pressed, false otherwise.
 */
bool ButtonIsDown(Button Button);

/*!
 * \brief Wait for the given button to be pressed.
 *
 * The function will return once a transition from the released to the pressed state occurs.
 *
 * \param btn Button to wait for.
 * \return Whether the operation succeeded or not.
 */
bool ButtonWaitForPress(Button btn);

/*!
 * \brief Wait for the given button to be pressed and then released (rtansition from rele).
 *
 * The function will return once a transition from the released to the pressed state and back occurs.
 *
 * \param btn Button to wait for.
 * \return Whether the operation succeeded or not.
 */
bool ButtonWaitForPressAndRelease(Button btn);

/*!
 * \brief Wait for any button press to happen.
 * \param timeout Maximum wait duration in milliseconds.
 * \return ButtonMask bitmask of pressed buttons.
 * \sa ButtonMask
 */
unsigned int ButtonWaitForAnyPress(unsigned int timeout);

/*!
 * \brief Wait for any button press or release to happen.
 * \param timeout Maximum wait duration in milliseconds.
 * \return ButtonMask of pressed buttons in bits 0-7,
 *         ButtonMask of released buttons in bits 15-8.
 */
unsigned int ButtonWaitForAnyEvent(unsigned int timeout);

#endif // ev3_button_lejos_h

#ifdef __cplusplus
}
#endif
