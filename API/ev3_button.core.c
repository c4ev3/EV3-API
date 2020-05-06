/** \file ev3_button.c
 * \brief Common low-level abstractions for EV3 buttons module
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
 * \date 2020-01-22
 * \version 1
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDES

#include "ev3.h"
#include "ev3_button.core.h"

#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES

/*!
 * \brief Memory structure shared between the kernel and the application.
 */
typedef struct
{
    //! Array containing button state (pressed = 1, released = 0).
    volatile uint8_t buttonPressed[NUM_BUTTONS];
} ButtonKernelMemory;

/*!
 * \brief Command structure for changing LED pattern that is sent to the kernel.
 */
typedef struct
{
    //! EV3 LED pattern, see LED_* constants
    LEDPattern ledPattern;
    //! necessary padding to 2 bytes
    uint8_t _padding;
} ButtonKernelWrite;

/*!
 * \brief Button module global data
 */
typedef struct
{
    //! Mmap'ed memory containing pressed buttons from the kernel
    ButtonKernelMemory *kernelMemory;

    //! FD of the LEGO kernel device
    int kernelFile;

    //! EV3 HW revision string (example: "V0.60")
    char hardwareRevision[8];

    //! Local lock
    pthread_mutex_t lock;

    //! Whether the module is currently initialized.
    bool initialized;
} ButtonCore;

//! Global button core data.
static ButtonCore buttonCore = {
        .kernelMemory = NULL,
        .kernelFile = -1,
        .hardwareRevision = {'\0'},
        .initialized = false,
        .lock = PTHREAD_MUTEX_INITIALIZER,
};

////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION

bool ButtonCoreInitialized(void)
{
    pthread_mutex_lock(&buttonCore.lock);
    bool retval = buttonCore.initialized;
    pthread_mutex_unlock(&buttonCore.lock);
    return retval;
}

bool ButtonCoreInit(void)
{
    pthread_mutex_lock(&buttonCore.lock);

    if (buttonCore.initialized)
        goto success;

    // open d_ui kernel device
    buttonCore.kernelFile = open(LMS_UI_DEVICE_NAME, O_RDWR | O_SYNC);
    if (buttonCore.kernelFile < 0)
    {
        perror("Cannot open kernel button device");
        goto failure;
    }

    // and map its memory to our process
    buttonCore.kernelMemory = mmap(0, sizeof(ButtonKernelMemory), PROT_READ, MAP_SHARED, buttonCore.kernelFile, 0);
    if (buttonCore.kernelMemory == MAP_FAILED)
    {
        buttonCore.kernelMemory = NULL;
        perror("Cannot mmap kernel button memory");
        goto failure;
    }

    // read string from d_ui
    int retval = read(buttonCore.kernelFile,
                      buttonCore.hardwareRevision,
                      sizeof(buttonCore.hardwareRevision) - 1);
    if (retval < 0)
    {
        perror("Cannot read brick HW revision");
        goto failure;
    }

success:
    buttonCore.initialized = true;
    pthread_mutex_unlock(&buttonCore.lock);
    return true;

failure:
    pthread_mutex_unlock(&buttonCore.lock);
    ButtonCoreExit();
    return false;
}

bool ButtonCoreExit(void)
{
    bool success = true;
    int retval;
    pthread_mutex_lock(&buttonCore.lock);

    // unmap memory
    if (buttonCore.kernelMemory != NULL)
    {
        retval = munmap(buttonCore.kernelMemory, sizeof(ButtonKernelMemory));
        if (retval < 0)
        {
            perror("Cannot unmap button kernel memory");
            success = false;
        }
        buttonCore.kernelMemory = NULL;
    }

    // and close the file
    if (buttonCore.kernelFile >= 0)
    {
        retval = close(buttonCore.kernelFile);
        if (retval < 0)
        {
            perror("Cannot close button kernel device");
            success = false;
        }
        buttonCore.kernelFile = -1;
    }

    buttonCore.initialized = false;
    pthread_mutex_unlock(&buttonCore.lock);
    return success;
}

const char *ButtonCoreReadHWRev(void)
{
    const char *retval = NULL;
    pthread_mutex_lock(&buttonCore.lock);

    if (buttonCore.initialized)
        retval = buttonCore.hardwareRevision;

    pthread_mutex_unlock(&buttonCore.lock);
    return retval;
}

bool ButtonCoreSetLEDs(LEDPattern pattern)
{
    bool success = false;

    if (pattern < LEDPattern_Off || pattern > LEDPattern_PulsingOrange)
        return false;

    ButtonKernelWrite kwrite = {.ledPattern = '0' + pattern};

    pthread_mutex_lock(&buttonCore.lock);

    if (buttonCore.initialized && buttonCore.kernelFile >= 0)
    {
        int retval = write(buttonCore.kernelFile, &kwrite, sizeof(kwrite));
        if (retval < 0)
        {
            perror("Setting LED pattern failed");
        }
        else
        {
            success = true;
        }
    }

    pthread_mutex_unlock(&buttonCore.lock);
    return success;
}

bool ButtonCoreReadButtons(uint8_t *buttonStates)
{
    bool success = false;
    pthread_mutex_lock(&buttonCore.lock);

    if (buttonCore.initialized && buttonCore.kernelMemory != NULL)
    {
        for (int i = 0; i < NUM_BUTTONS; i++)
            buttonStates[i] = buttonCore.kernelMemory->buttonPressed[i];
        success = true;
    }

    pthread_mutex_unlock(&buttonCore.lock);
    return success;
}

int ButtonCoreReadButton(Button button)
{
    int retval = -1;

    if (button < 0 || button >= NUM_BUTTONS)
        return -1;

    pthread_mutex_lock(&buttonCore.lock);

    if (buttonCore.initialized && buttonCore.kernelMemory != NULL)
        retval = buttonCore.kernelMemory->buttonPressed[button] != 0;

    pthread_mutex_unlock(&buttonCore.lock);
    return retval;
}
