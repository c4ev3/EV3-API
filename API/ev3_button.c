/*
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
 * \author Simón Rodriguez Perez(Hochschule Aschaffenburg)
 * \date 2016-04-19
 * \version 2
 * \note Correction of function name [void ButtonWaitForPressAndRelease(byte Button)]
 */

#include "ev3_button.h"
#include "ev3_timer.h"

#define BUTTON_DEBOUNCE_TIME 10
#define BUTTON_POLL_TIME     50

#define PRESS_EVENT_SHIFT     0
#define RELEASE_EVENT_SHIFT   8
#define WAITFOR_RELEASE_SHIFT 8

#define HWVERS_SIZE 6

typedef struct {
	char Pressed[NUM_BUTTONS];
} BUTTON;

typedef struct {
	BUTTON ButtonSafe;
	BUTTON* pButton;
	int ButtonLedFile;
	uint8_t LedPattern;
	bool WarnLight;
	uint16_t curButtonsS;
	uint16_t curButtonsE;
} ButtonLedGlobals;

ButtonLedGlobals ButtonLedInstance;

char HwVers[HWVERS_SIZE];

int WriteToButtonLedDevice(uint8_t * bytes, int num_bytes)
{
	int result = -1;
	if (ButtonLedInstance.ButtonLedFile >= 0)
	{
		// for some reason write is not returning num_bytes -
		// it usually returns zero
		result = write(ButtonLedInstance.ButtonLedFile, bytes, num_bytes);
		if (result >= 0)
			return num_bytes;
	}
	return result;
}

uint16_t checkButtons()
{
	int i;
	uint8_t p;
	uint16_t Result = 0;
	for (i = 0; i < NUM_BUTTONS; i++)
	{
		p = (*ButtonLedInstance.pButton).Pressed[i];
		if (p != 0)
		{
			Result |= (1 << i);
		}
	}
	return Result;
}

uint16_t getButtons()
{
	uint16_t state1, state2;
	// read buttons and de-bounce them
	do
	{
		state1 = checkButtons();
		usleep(BUTTON_DEBOUNCE_TIME*1000);
		state2 = checkButtons();
	} while (state1 != state2);
	return state1;
}

void buttonClear()
{
	// Initialize with state of buttons instead of zero. That prevents false
	// press events at program start
	ButtonLedInstance.curButtonsS = getButtons();
	ButtonLedInstance.curButtonsE = ButtonLedInstance.curButtonsS;
}

uint16_t readButtons()
{
	uint16_t Result = getButtons();
	ButtonLedInstance.curButtonsS = Result;
	return Result;
}

bool ButtonLedInitialized()
{
	return (ButtonLedInstance.ButtonLedFile != -1) &&
		   (ButtonLedInstance.pButton != NULL);
}

void ButtonLedCloseDevices()
{
	if (!ButtonLedInitialized())
		return;

	ButtonLedInstance.WarnLight = false;

	if ((ButtonLedInstance.pButton != NULL) &&
		(ButtonLedInstance.pButton != &ButtonLedInstance.ButtonSafe))
	{
		munmap(ButtonLedInstance.pButton, sizeof(BUTTON));
	}

	if (ButtonLedInstance.ButtonLedFile >= 0)
	{
		SetLedPattern(LED_BLACK);
		close(ButtonLedInstance.ButtonLedFile);
	}

	ButtonLedInstance.ButtonLedFile = -1;
	ButtonLedInstance.pButton = NULL;
}

bool ButtonLedInit()
{
	if (ButtonLedInitialized())
		return true;

	ButtonLedInstance.pButton = &ButtonLedInstance.ButtonSafe;

	ButtonLedInstance.ButtonLedFile = open(LMS_UI_DEVICE_NAME, O_RDWR | O_SYNC);

	// if any of our devices didn't open successfully then
	// make sure they are all closed and exit false
	if (ButtonLedInstance.ButtonLedFile < 0)
	{
		ButtonLedCloseDevices();
		return false;
	}

	BUTTON* pButtonTmp = (BUTTON*)mmap(0, sizeof(BUTTON), PROT_READ + PROT_WRITE, MAP_SHARED, ButtonLedInstance.ButtonLedFile, 0);
	if (pButtonTmp == MAP_FAILED)
	{
//    LogErrorNumber(UI_SHARED_MEMORY);
		ButtonLedCloseDevices();
		return false;
	}
	else
	{
		ButtonLedInstance.pButton = pButtonTmp;
	}

	buttonClear();
	ButtonLedInstance.WarnLight = false;
	ButtonLedInstance.LedPattern = NUM_LED_PATTERNS; // impossible value
	SetLedPattern(LED_BLACK);

	return true;
}

bool ButtonLedOpen()
{
	if (!ButtonLedInitialized())
		return false;

	buttonClear();

	return true;
}

bool ButtonLedClose()
{
	if (!ButtonLedInitialized())
		return false;

	buttonClear();

	return true;
}

bool ButtonLedExit()
{
	// if not initialized then just exit
	if (!ButtonLedInitialized())
		return true;
	ButtonLedClose();
	ButtonLedCloseDevices();

	return true;
}

float HardwareVersion()
{
	float Result = 0.0;

	if (!ButtonLedInitialized())
		return Result;

	read(ButtonLedInstance.ButtonLedFile, HwVers, HWVERS_SIZE);
	sscanf(&HwVers[1], "%f", &Result);
	Result *= 10.0;
	return Result;
}

char* HardwareVersionString()
{
	if (!ButtonLedInitialized())
		return "";

	read(ButtonLedInstance.ButtonLedFile, HwVers, HWVERS_SIZE);
	return HwVers;
}

uint8_t MappedPattern(uint8_t Pattern)
{
	if (ButtonLedInstance.WarnLight)
	{
		if ((Pattern == LED_GREEN_FLASH) ||
			(Pattern == LED_RED_FLASH) ||
			(Pattern == LED_ORANGE_FLASH))
		{
			Pattern = LED_ORANGE_FLASH;
		}
		else if ((Pattern == LED_GREEN_PULSE) ||
				 (Pattern == LED_RED_PULSE) ||
				 (Pattern == LED_ORANGE_PULSE))
		{
			Pattern = LED_ORANGE_PULSE;
		}
		else
		{
			Pattern = LED_ORANGE;
		}
	}
	return Pattern;
}

void UpdateLed(uint8_t Pattern)
{
	if (!ButtonLedInitialized())
		return;

	const int cmdLen = 2;
	const uint8_t zero = 48; // ASCII value of '0'
	uint8_t cmd[2];

	if (Pattern < NUM_LED_PATTERNS)
	{
		ButtonLedInstance.LedPattern = Pattern;
		cmd[0] = zero + MappedPattern(Pattern);
		cmd[1] = 0;
		WriteToButtonLedDevice(cmd, cmdLen);
	}
}

void SetLedWarning(bool Value)
{
	if (!ButtonLedInitialized())
		return;

	if (Value != ButtonLedInstance.WarnLight)
	{
		ButtonLedInstance.WarnLight = Value;
		UpdateLed(ButtonLedInstance.LedPattern);
	}
}

uint8_t LedWarning()
{
	if (!ButtonLedInitialized())
		return false;
	return ButtonLedInstance.WarnLight;
}

void SetLedPattern(uint8_t Pattern)
{
	if (!ButtonLedInitialized())
		return;

	if (Pattern != ButtonLedInstance.LedPattern)
	{
		UpdateLed(Pattern);
	}
}

uint8_t LedPattern()
{
	if (!ButtonLedInitialized())
		return LED_BLACK;
	return MappedPattern(ButtonLedInstance.LedPattern);
}

uint16_t ButtonWaitForAnyEvent(unsigned int timeout)
{
	if (!ButtonLedInitialized())
		return 0;

	unsigned long long endms, curTime;
	uint16_t oldDown, newDown;

	if (timeout == 0)
		endms = 0x7fffffffffffffffULL;
	else
		endms = TimerGetMS() + timeout;
	oldDown = ButtonLedInstance.curButtonsE;
	while (true)
	{
		curTime = TimerGetMS();
		if (curTime >= endms)
		{
			return 0;
		}
		usleep(BUTTON_POLL_TIME*1000);
		ButtonLedInstance.curButtonsE = readButtons();
		newDown = ButtonLedInstance.curButtonsE;
		if (newDown != oldDown)
		{
			return ((oldDown & ~newDown) << WAITFOR_RELEASE_SHIFT) | (newDown & ~oldDown);
		}
	}
}

uint16_t ButtonWaitForAnyPress(unsigned int timeout)
{
	if (!ButtonLedInitialized())
		return 0;

	unsigned long long endms, curTime;
	uint16_t oldDown, newDown, pressed;

	if (timeout == 0)
		endms = 0x7fffffffffffffffULL;
	else
		endms = TimerGetMS() + timeout;
	oldDown = ButtonLedInstance.curButtonsE;
	while (true)
	{
		curTime = TimerGetMS();
		if (curTime >= endms)
		{
			return 0;
		}
		usleep(BUTTON_POLL_TIME*1000);
		ButtonLedInstance.curButtonsE = readButtons();
		newDown = ButtonLedInstance.curButtonsE;
		pressed = newDown & ~oldDown;
		if (pressed != 0)
		{
			return pressed;
		}
		oldDown = newDown;
	}
}

bool ButtonIsUp(uint8_t Button)
{
	return (readButtons() & Button) == 0;
}

bool ButtonIsDown(uint8_t Button)
{
	return !ButtonIsUp(Button);
}

void ButtonWaitForPress(uint8_t Button)
{
	while ((ButtonWaitForAnyPress(0) & Button) == 0)
	{
		// wait for next press
	}
}

void ButtonWaitForPressAndRelease(uint8_t Button)
{
	uint16_t tmp;
	ButtonWaitForPress(Button);
	tmp = (uint16_t)Button << WAITFOR_RELEASE_SHIFT;
	while ((ButtonWaitForAnyEvent(0) & tmp) == 0)
	{
		// wait for next event
	}
}

// NXC-style API functions (no support for short press, long press,
// short release, long release, or press counts
bool ButtonPressedEx(uint8_t btn, bool resetCount)
{
	// resetCount parameter is ignored
	return ButtonIsDown(btn);
}

char ReadButtonEx(uint8_t btn, bool reset, bool* pressed, uint16_t* count)
{
	// reset parameter is ignored
	*count = 0;
	*pressed = ButtonIsDown(btn);
	return 0; // okay
}

uint8_t ButtonState(uint8_t btn)
{
	if (ButtonIsDown(btn))
		return BTNSTATE_PRESSED_STATE | BTNSTATE_PRESSED_EV;
	else
		return BTNSTATE_NONE;
}

