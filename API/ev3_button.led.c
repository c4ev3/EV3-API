/** \file ev3_button.led.c
 * \brief Implementation of the EV3 LED interaction
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
#include "ev3.h"
#include "ev3_button.led.h"


////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES

typedef struct
{
    pthread_mutex_t lock;

    LEDPattern currentPattern;
    bool warningActive;
    bool initialized;
} LEDs;

static LEDs leds = {
        .lock = PTHREAD_MUTEX_INITIALIZER,
        .currentPattern = LEDPattern_Off,
        .warningActive = false,
        .initialized = false,
};

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION DECLARATIONS

/*!
 * \brief Upload new LED pattern to the kernel.
 *
 * The value to set will be taken from the global button module memory.
 *
 * \return Whether the operation succeeded or not.
 */
static bool btnRefreshLeds(void);

////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION

bool LedInitialized(void)
{
    pthread_mutex_lock(&leds.lock);
    bool retval = leds.initialized;
    pthread_mutex_unlock(&leds.lock);
    return retval;
}

bool LedInit(void)
{
    pthread_mutex_lock(&leds.lock);

    if (leds.initialized)
        goto success;

    if (!ButtonCoreInit())
        goto failure;

    leds.warningActive = false;
    leds.currentPattern = LEDPattern_PulsingGreen;
    if (!btnRefreshLeds())
        goto failure;

success:
    leds.initialized = true;
    pthread_mutex_unlock(&leds.lock);
    return true;

failure:
    pthread_mutex_unlock(&leds.lock);
    LedExit();
    return false;
}

bool LedExit(void)
{
    bool success;
    pthread_mutex_lock(&leds.lock);

    if (leds.initialized)
        success = ButtonCoreSetLEDs(LEDPattern_SteadyGreen);
    else
        success = true;

    leds.initialized = false;

    pthread_mutex_unlock(&leds.lock);
    return success;
}

LEDPattern LedPattern()
{
    LEDPattern retval = LEDPattern_Off;

    pthread_mutex_lock(&leds.lock);
    if (leds.initialized)
        retval = leds.currentPattern;
    pthread_mutex_unlock(&leds.lock);

    return retval;
}

bool SetLedPattern(LEDPattern newPattern)
{
    bool success = false;

    if (newPattern < LEDPattern_Off || newPattern > LEDPattern_PulsingOrange)
        return false;

    pthread_mutex_lock(&leds.lock);
    if (leds.initialized)
    {
        if (leds.currentPattern != newPattern)
        {
            leds.currentPattern = newPattern;
            success = btnRefreshLeds();
        }
        else
        {
            success = true;
        }
    }
    pthread_mutex_unlock(&leds.lock);

    return success;
}

bool LedWarning()
{
    bool retval = false;

    pthread_mutex_lock(&leds.lock);
    if (leds.initialized)
        retval = leds.warningActive;
    pthread_mutex_unlock(&leds.lock);

    return retval;
}

bool SetLedWarning(bool doWarn)
{
    bool success = false;

    pthread_mutex_lock(&leds.lock);
    if (leds.initialized)
    {
        if (leds.warningActive != doWarn)
        {
            leds.warningActive = doWarn;
            success = btnRefreshLeds();
        }
        else
        {
            success = true;
        }
    }
    pthread_mutex_unlock(&leds.lock);

    return success;
}

bool btnRefreshLeds(void)
{
    LEDPattern patternToUpload = leds.currentPattern;
    if (leds.warningActive)
    {
        switch (patternToUpload)
        {
            case LEDPattern_Off:
                patternToUpload = LEDPattern_Off;
                break;
            default: // fallthrough
            case LEDPattern_SteadyGreen: // fallthrough
            case LEDPattern_SteadyRed: // fallthrough
            case LEDPattern_SteadyOrange:
                patternToUpload = LEDPattern_SteadyOrange;
                break;
            case LEDPattern_FlashingGreen: // fallthrough
            case LEDPattern_FlashingRed:  // fallthrough
            case LEDPattern_FlashingOrange:
                patternToUpload = LEDPattern_FlashingOrange;
                break;
            case LEDPattern_PulsingGreen: // fallthrough
            case LEDPattern_PulsingRed: // fallthrough
            case LEDPattern_PulsingOrange:
                patternToUpload = LEDPattern_PulsingOrange;
                break;
        }
    }
    return ButtonCoreSetLEDs(patternToUpload);
}
