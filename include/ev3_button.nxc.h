/** \file ev3_button.nxc.h
 * \brief NXC-like API for accessing brick buttons.
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

#ifndef ev3_button_nxc_h
#define ev3_button_nxc_h

#include "ev3_constants.h"
#include <stdbool.h>

//! NXC syscall definition
typedef struct
{
    //! [OUT] How many times the button has been pressed.
    unsigned int Count;
    //! [IN] Button index
    Button Index;
    //! [OUT] Whether the syscall succeeded.
    bool Result;
    //! [IN] Whether the button press count should be reset.
    bool Reset;
    //! [OUT] Whether the button is currently pressed.
    bool Pressed;
} ReadButtonType;

/*!
 * \brief Initialize the NXC button API.
 * \return Whether the initialization completed successfully or not.
 */
bool ButtonNxcInit();

/*!
 * \brief Deinitialize the NXC button API.
 * \return Whether the deinitialization completed successfully or not.
 */
bool ButtonNxcExit();

/*!
 * \brief Disable periodic button processing.
 * \remark This will render this module inoperable (everything it does
 *         depends on periodic 10ms callback being called).
 * \return Whether the suspend succeeded or not.
 */
bool ButtonNxcSuspend();

/*!
 * \brief Reenable periodic button processing.
 * \return Whether the resume succeeded or not.
 */
bool ButtonNxcResume();

/*!
 * \brief Check whether the NXC button API is initialized.
 * \return True if yes, false otherwise.
 */
bool ButtonNxcInitialized();

/*!
 * \brief Check whether the given button is currently pressed.
 * \param btn Button to check.
 * \return True if the button is pressed, false otherwise.
 */
bool ButtonPressed(Button btn);

/*!
 * \brief Check whether the given button is currently pressed, optionally resetting the button press count.
 * \param btn Button to check.
 * \param resetCount If set to true, the button press count will be reset.
 * \return True if the button is pressed, false otherwise.
 */
bool ButtonPressedEx(Button btn, bool resetCount);

/*!
 * \brief Check how many times a button was pressed.
 * \param btn Button to check.
 * \return How many times the button was pressed since the last time the counter was reset.
 */
unsigned int ButtonCount(Button btn);

/*!
 * \brief Check how many times a button was pressed.
 * \param btn Button to check.
 * \param resetCount If set to true, the button press count will be reset.
 * \return How many times the button was pressed since the last time the counter was reset.
 */
unsigned int ButtonCountEx(Button btn, bool resetCount);

/*!
 * \brief Read and/or modify multiple attributes of a button.
 * \param btn Button to check/modify.
 * \param reset Whether the press count should be reset.
 * \param[out] pressed Whether the button is currently pressed. Can be NULL.
 * \param[out] count How many times the button was pressed since last reset. Can be NULL.
 * \return Errno value; 0 means no error.
 */
int ReadButtonEx(Button btn, bool reset, bool *pressed, unsigned int *count);

/*!
 * \brief Check how many times a button was pressed.
 * \param btn Button to check.
 * \return How many times the button was pressed since the last time the counter was reset.
 */
unsigned int ButtonPressCount(Button btn);

/*!
 * \brief Check how many times a button was long-pressed.
 * \param btn Button to check.
 * \return How many times the button was long-pressed since the last time the counter was reset.
 */
unsigned int ButtonLongPressCount(Button btn);

/*!
 * \brief Check how many times a button was released from a short-press.
 * \param btn Button to check.
 * \return How many times the button was released from a short-press since the last time the counter was reset.
 */
unsigned int ButtonShortReleaseCount(Button btn);

/*!
 * \brief Check how many times a button was released from a long-press.
 * \param btn Button to check.
 * \return How many times the button was released from a long-press since the last time the counter was reset.
 */
unsigned int ButtonLongReleaseCount(Button btn);

/*!
 * \brief Check how many times a button was released.
 * \param btn Button to check.
 * \return How many times the button was released since the last time the counter was reset.
 */
unsigned int ButtonReleaseCount(Button btn);

/*!
 * \brief Query current button press state.
 * \param btn Button to query.
 * \return Press detection state for the given butotn.
 * \sa BTNSTATE
 */
ButtonStates ButtonState(Button btn);

/*!
 * \brief Set new value for the long-press button counter.
 * \param btn Button where to modify the counter.
 * \param count New value of the counter.
 */
void SetButtonLongPressCount(Button btn, unsigned int count);

/*!
 * \brief Set new value for the long-release button counter.
 * \param btn Button where to modify the counter.
 * \param count New value of the counter.
 */
void SetButtonLongReleaseCount(Button btn, unsigned int count);

/*!
 * \brief Set new value for the press button counter.
 * \param btn Button where to modify the counter.
 * \param count New value of the counter.
 */
void SetButtonPressCount(Button btn, unsigned int count);

/*!
 * \brief Set new value for the release button counter.
 * \param btn Button where to modify the counter.
 * \param count New value of the counter.
 */
void SetButtonReleaseCount(Button btn, unsigned int count);

/*!
 * \brief Set new value for the short-release button counter.
 * \param btn Button where to modify the counter.
 * \param count New value of the counter.
 */
void SetButtonShortReleaseCount(Button btn, unsigned int count);

/*!
 * \brief Set new value for the button state.
 * \param btn Button where to modify the state.
 * \param count New value of the state.
 * \remark This function is probably not very useful, as
 *         the state will be overwritten by the detection loop.
 */
void SetButtonState(Button btn, ButtonStates state);

/*!
 * \brief Invoke a NXC syscall for reading button state.
 * \param args Syscall arguments.
 */
void SysReadButton(ReadButtonType *args);

/*!
 * \brief Set a bitmask of button states that cause the program to exit.
 * \param bitmask If a bitwise AND of this value and the button state is not zero, the program is exited at that time.
 */
void SetAbortFlag(ButtonStates bitmask);

/*!
 * \brief Set whether a long escape button press is required in order to exit the program.
 * \param longAbort True for long press, false for short press.
 */
void SetLongAbort(bool longAbort);


#endif // ev3_button_nxc_h

#ifdef __cplusplus
}
#endif
