/** \file ev3_button.nxc.c
 * \brief Frontend implementation of the NXC button API
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
 * \date 2020-02-12
 * \version 1
 */

#include <pthread.h>
#include <signal.h>
#include "ev3.h"
#include "ev3_button.nxc.h"


////////////////////////////////////////////////////////////////////////////////
// MACROS

//! Timer interval enum value for the button handler
#define BUTTON_HANDLER_PERIOD_ENUM ti10ms
//! Timer interval value for the button handler
#define BUTTON_HANDLER_PERIOD 10 // msec

//! Number of loops that have to elapse before a button press is registered as a long press
#define LONG_EVENT_LOOPS  (2000/BUTTON_HANDLER_PERIOD)

////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES

/*!
 * \brief Button module global data
 */
typedef struct
{
    //! Mutex used by the NXC-style button accessors & event loop
    pthread_mutex_t lock;

    //! How many times was a button pressed.
    unsigned int pressCount[NUM_BUTTONS];
    //! How many times was a button long-pressed.
    unsigned int longPressCount[NUM_BUTTONS];
    //! How many times was a button released.
    unsigned int shortReleaseCount[NUM_BUTTONS];
    //! How many times was a button long-released.
    unsigned int longReleaseCount[NUM_BUTTONS];
    //! How many times was a button released (in any way).
    unsigned int releaseCount[NUM_BUTTONS];

    //! For how many loops has the state in 'buttonPressed' continued.
    unsigned int pressedLoopCount[NUM_BUTTONS];

    //! Current NXT/NXC button state flag.
    ButtonStates nxcState[NUM_BUTTONS];
    //! BTNEXIT state mask for program termination.
    ButtonStates abortFlag;
    //! Current button press state before debouncing.
    bool lastKmem[NUM_BUTTONS];
    //! Current button press sButtonStatestate after debouncing.
    bool debouncedPressed[NUM_BUTTONS];

    //! Whether the periodic timer handler is registered.
    bool installed;
    //! Whether the module is currently initialized.
    bool initialized;
} ButtonControlBlock;

//! Global button module data.
static ButtonControlBlock bcb = {
        .initialized = false,
        .installed = false,
        .lock = PTHREAD_MUTEX_INITIALIZER,
};

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION DECLARATIONS

/*!
 * \brief Prepare the global button-related state.
 * Requires access to the kernel.
 * \return Whether the operation succeeded or not.
 */
static bool btnInitStateMachine(void);

/*!
 * \brief Update the global state of buttons.
 */
static void btnPeriodicHandler(int);

/*!
 * \brief Debounce buttons in the global data.
 * \param button Button index to check.
 * \param kmem Current button status from the kernel.
 */
static void btnDebounceButton(Button btn, const uint8_t *kmem);

/*!
 * \brief Safely load the given U16 button property.
 * \param button Button index.
 * \param array Button array.
 * \return Current property value.
 */
static unsigned int AtomicLoadHelperUInt(Button button, const unsigned int *array);

/*!
 * \brief Safely store the given U16 button property.
 * \param button Button index.
 * \param array Button array.
 * \param value Value to set.
 */
static void AtomicStoreHelperUInt(Button button, unsigned int *array, unsigned int value);

////////////////////////////////////////////////////////////////////////////////
// LIFETIME

bool ButtonNxcInitialized()
{
    pthread_mutex_lock(&bcb.lock);
    bool retval = bcb.initialized;
    pthread_mutex_unlock(&bcb.lock);
    return retval;
}

bool ButtonNxcInit()
{
    pthread_mutex_lock(&bcb.lock);

    if (bcb.initialized)
        goto success;

    if (!TimerInit())
        goto failure;

    if (!ButtonCoreInit())
        goto failure;

    if (!btnInitStateMachine())
        goto failure;

    if (!SetTimerCallback(BUTTON_HANDLER_PERIOD_ENUM, btnPeriodicHandler))
        goto failure;
    else
        bcb.installed = true;

success:
    bcb.initialized = true;
    pthread_mutex_unlock(&bcb.lock);
    return true;

failure:
    pthread_mutex_unlock(&bcb.lock);
    ButtonNxcExit();
    return false;
}

bool ButtonNxcExit()
{
    bool success = true;
    pthread_mutex_lock(&bcb.lock);

    if (bcb.installed)
        success = RemoveTimerCallback(BUTTON_HANDLER_PERIOD_ENUM, btnPeriodicHandler);

    if (success)
        bcb.installed = false;

    bcb.initialized = false;

    pthread_mutex_unlock(&bcb.lock);
    return success;
}

bool ButtonNxcSuspend()
{
    bool success = true;
    pthread_mutex_lock(&bcb.lock);

    if (bcb.installed)
        success = RemoveTimerCallback(BUTTON_HANDLER_PERIOD_ENUM, btnPeriodicHandler);

    if (success)
        bcb.installed = false;

    pthread_mutex_unlock(&bcb.lock);
    return success;
}

bool ButtonNxcResume()
{
    bool success = true;
    pthread_mutex_lock(&bcb.lock);

    if (!bcb.installed)
        success = SetTimerCallback(BUTTON_HANDLER_PERIOD_ENUM, btnPeriodicHandler);

    if (success)
        bcb.installed = true;

    pthread_mutex_unlock(&bcb.lock);
    return success;
}

////////////////////////////////////////////////////////////////////////////////
// BUTTON PERIODIC HANDLER

bool btnInitStateMachine(void)
{
    uint8_t buttons[NUM_BUTTONS];
    if (!ButtonCoreReadButtons(buttons))
        return false;

    for (Button btn = 0; btn < NUM_BUTTONS; btn++)
    {
        // set initial state to prevent ghost presses
        bool pressed = buttons[btn] != 0;
        bcb.lastKmem[btn] = pressed;
        bcb.debouncedPressed[btn] = pressed;
        if (pressed)
        {
            bcb.nxcState[btn] = ButtonStates_PressedState;
            bcb.pressedLoopCount[btn] = LONG_EVENT_LOOPS + 1;
        }
        else
        {
            bcb.nxcState[btn] = ButtonStates_ReleasedState;
            bcb.pressedLoopCount[btn] = 0;
        }

        // zero out counters
        bcb.pressCount[btn] = 0;
        bcb.longPressCount[btn] = 0;
        bcb.shortReleaseCount[btn] = 0;
        bcb.longReleaseCount[btn] = 0;
        bcb.releaseCount[btn] = 0;
    }
    // default behaviour = exit immediately after the back key is pressed
    bcb.abortFlag = BTNSTATE_PRESSED_EV;
    return true;
}

void btnPeriodicHandler(int interval)
{
    (void) interval;

    pthread_mutex_lock(&bcb.lock);
    if (!bcb.initialized)
    {
        pthread_mutex_unlock(&bcb.lock);
        return;
    }

    // copy kernel memory
    uint8_t buttons[NUM_BUTTONS];
    if (!ButtonCoreReadButtons(buttons))
        return;

    // for all buttons
    for (Button btn = 0; btn < NUM_BUTTONS; btn++)
    {
        // debounce & handle "signal edges"
        bool was = bcb.debouncedPressed[btn];
        btnDebounceButton(btn, buttons);
        bool now = bcb.debouncedPressed[btn];

        if (now)
        {
            if (!was) // new press
            {
                bcb.pressedLoopCount[btn] = 0;
                bcb.nxcState[btn] = ButtonStates_PressedState | ButtonStates_PressEvent;
                bcb.pressCount[btn]++;
            }
            else // continued press
            {
                if (bcb.pressedLoopCount[btn] < LONG_EVENT_LOOPS) // short press
                {
                    bcb.nxcState[btn] = ButtonStates_PressedState;
                    bcb.pressedLoopCount[btn]++;
                }
                else if (bcb.pressedLoopCount[btn] == LONG_EVENT_LOOPS) // long press
                {
                    bcb.nxcState[btn] = ButtonStates_PressedState | ButtonStates_LongPressEvent;
                    bcb.pressedLoopCount[btn]++;
                    bcb.longPressCount[btn]++;
                }
                else if (bcb.pressedLoopCount[btn] > LONG_EVENT_LOOPS) // past long press
                {
                    bcb.nxcState[btn] = ButtonStates_PressedState;
                }
            }
        }
        else // !now
        {
            if (was) // new release
            {
                if (bcb.pressedLoopCount[btn] >= LONG_EVENT_LOOPS) // was long press
                {
                    bcb.nxcState[btn] = ButtonStates_ReleasedState | ButtonStates_LongReleaseEvent;
                    bcb.releaseCount[btn]++;
                    bcb.longReleaseCount[btn]++;
                }
                else // was short press
                {
                    bcb.nxcState[btn] = ButtonStates_ReleasedState | ButtonStates_ShortReleaseEvent;
                    bcb.releaseCount[btn]++;
                    bcb.shortReleaseCount[btn]++;
                }
            }
            else // !was; continued release
            {
                bcb.nxcState[btn] = ButtonStates_ReleasedState;
            }
        }
    }

    bool doExit = (bcb.nxcState[Button_Escape] & bcb.abortFlag) != 0;

    pthread_mutex_unlock(&bcb.lock);

    if (doExit)
    {
        // ensure that there is no ghost click in lms2012
        while (ButtonCoreReadButton(Button_Escape) == 1)
            usleep(10);

        exit(0);
    }
}

void btnDebounceButton(Button btn, const uint8_t *kmem)
{
    // handle old/new raw button state
    bool wasRaw = bcb.lastKmem[btn];
    bool nowRaw = kmem[btn];
    bcb.lastKmem[btn] = nowRaw;

    // if button state is unchanged, set the current button state as the debounced one
    // otherwise keep the old state as the debounced one
    if (nowRaw == wasRaw)
        bcb.debouncedPressed[btn] = nowRaw;
}


////////////////////////////////////////////////////////////////////////////////
// NXC BUTTON API

// abort flags

void SetAbortFlag(ButtonStates bitmask)
{
    pthread_mutex_lock(&bcb.lock);
    if (bcb.initialized)
        bcb.abortFlag = bitmask;
    pthread_mutex_unlock(&bcb.lock);
}

void SetLongAbort(bool longAbort)
{
    SetAbortFlag(longAbort ? ButtonStates_LongPressEvent : ButtonStates_PressEvent);
}

// universal read-all function
int ReadButtonEx(Button btn, bool reset, bool *pressed, unsigned int *count)
{
    if (btn >= NUM_BUTTONS)
        return EXIT_FAILURE;

    pthread_mutex_lock(&bcb.lock);
    if (bcb.initialized)
    {
        if (pressed)
            *pressed = (bcb.nxcState[btn] & ButtonStates_PressedState) != 0;
        if (count)
            *count = bcb.pressCount[btn];
        if (reset)
            bcb.pressCount[btn] = 0;
    }
    pthread_mutex_unlock(&bcb.lock);

    return EXIT_SUCCESS;
}

bool ButtonPressedEx(Button btn, bool resetCount)
{
    bool pressed = false;
    ReadButtonEx(btn, resetCount, &pressed, NULL);
    return pressed;
}

unsigned int ButtonCountEx(Button btn, bool resetCount)
{
    unsigned int count = 0;
    ReadButtonEx(btn, resetCount, NULL, &count);
    return count;
}

bool ButtonPressed(Button btn)
{
    return (ButtonState(btn) & ButtonStates_PressedState) != 0;
}

unsigned int ButtonCount(Button btn)
{
    return ButtonPressCount(btn);
}

unsigned int ButtonPressCount(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.pressCount);
}

unsigned int ButtonLongPressCount(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.longPressCount);
}

unsigned int ButtonShortReleaseCount(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.shortReleaseCount);
}

unsigned int ButtonLongReleaseCount(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.longReleaseCount);
}

unsigned int ButtonReleaseCount(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.releaseCount);
}

ButtonStates ButtonState(Button btn)
{
    return AtomicLoadHelperUInt(btn, bcb.nxcState);
}

void SetButtonLongPressCount(Button btn, unsigned int count)
{
    return AtomicStoreHelperUInt(btn, bcb.longPressCount, count);
}

void SetButtonLongReleaseCount(Button btn, unsigned int count)
{
    return AtomicStoreHelperUInt(btn, bcb.longReleaseCount, count);
}

void SetButtonPressCount(Button btn, unsigned int count)
{
    return AtomicStoreHelperUInt(btn, bcb.longPressCount, count);
}

void SetButtonReleaseCount(Button btn, unsigned int count)
{
    return AtomicStoreHelperUInt(btn, bcb.releaseCount, count);
}

void SetButtonShortReleaseCount(Button btn, unsigned int count)
{
    return AtomicStoreHelperUInt(btn, bcb.shortReleaseCount, count);
}

void SetButtonState(Button btn, ButtonStates state)
{
    return AtomicStoreHelperUInt(btn, bcb.nxcState, state);
}

void SysReadButton(ReadButtonType *args)
{
    if (!args)
        return;

    args->Result = ReadButtonEx(args->Index, args->Reset, &args->Pressed, &args->Count);
}

////////////////////////////////////////////////////////////////////////////////
// ATOMIC HELPERS

unsigned int AtomicLoadHelperUInt(Button button, const unsigned int *array)
{
    if (button >= NUM_BUTTONS)
        return false;

    unsigned int retval = 0;
    pthread_mutex_lock(&bcb.lock);

    if (bcb.initialized)
        retval = array[button];

    pthread_mutex_unlock(&bcb.lock);
    return retval;
}

void AtomicStoreHelperUInt(Button button, unsigned int *array, unsigned int value)
{
    if (button >= NUM_BUTTONS)
        return;

    pthread_mutex_lock(&bcb.lock);

    if (bcb.initialized)
        array[button] = value;

    pthread_mutex_unlock(&bcb.lock);
}
