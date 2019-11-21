/** \file ev3_output.h
 * \brief Functions and constants for controlling EV3 outputs
 *
 * ev3_output.h contains declarations for the EV3 C API output functions.
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
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ev3_output_h
#define ev3_output_h

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <limits.h>

#include "ev3_wait.h"
#include "ev3_constants.h"


// Gets the tacho count used in sensor mode
bool OutputGetCount(uint8_t Output, int * Tacho);

// Read steps from last reset
bool OutputGetTachoCount(uint8_t Output, int * Tacho);

// Read actual speed
bool OutputGetActualSpeed(uint8_t Output, int8_t * Speed);

void OffEx(uint8_t Outputs, uint8_t reset);

void FloatEx(uint8_t Outputs, uint8_t reset);

static inline void Off(uint8_t _outputs) {
    OffEx(_outputs, RESET_NONE);
}

static inline void Float(uint8_t _outputs) {
    FloatEx(_outputs, RESET_NONE);
}

void OnFwdEx(uint8_t Outputs, int8_t Power, uint8_t reset);

static inline void OnRevEx(uint8_t Outputs, int8_t Power, uint8_t reset) {
    OnFwdEx(Outputs, -Power, reset);
}

void OnFwdRegEx(uint8_t Outputs, int8_t Speed, uint8_t RegMode, uint8_t reset);

static inline void OnRevRegEx(uint8_t Outputs, int8_t Speed, uint8_t RegMode, uint8_t reset) {
    OnFwdRegEx(Outputs, -Speed, RegMode, reset);
}

static inline void OnFwdReg(uint8_t _outputs, int8_t _speed) {
    OnFwdRegEx(_outputs, _speed, OUT_REGMODE_SPEED, RESET_NONE);
}

static inline void OnRevReg(uint8_t _outputs, int8_t _speed) {
    OnRevRegEx(_outputs, _speed, OUT_REGMODE_SPEED, RESET_NONE);
}

void OnFwdSyncEx(uint8_t Outputs, int8_t Speed, short Turn, uint8_t reset);

static inline void OnRevSyncEx(uint8_t Outputs, int8_t Speed, short Turn, uint8_t reset) {
    OnFwdSyncEx(Outputs, -Speed, Turn, reset);
}

static inline void OnFwdSync(uint8_t  _outputs, int8_t _speed) {
    OnFwdSyncEx(_outputs, _speed, 0, RESET_NONE);
}

static inline void OnRevSync(uint8_t  _outputs, int8_t _speed) {
    OnRevSyncEx(_outputs, _speed, 0, RESET_NONE);
}

void RotateMotorNoWaitEx(uint8_t Outputs, int8_t Speed, int Angle, short Turn, bool Sync, bool Stop);

static inline void RotateMotorNoWait(uint8_t _outputs, int8_t _speed, int _angle) {
    RotateMotorNoWaitEx(_outputs, _speed, _angle, 0, true, true);
}

void RotateMotorEx(uint8_t Outputs, int8_t Speed, int Angle, short Turn, bool Sync, bool Stop);

static inline void RotateMotor(uint8_t _outputs, int8_t _speed, int _angle) {
    RotateMotorEx(_outputs, _speed, _angle, 0, true, true);
}

void OnForSyncEx(uint8_t Outputs, int Time, int8_t Speed, short Turn, bool Stop);

static inline void OnForSync(uint8_t _outputs, int _time, int _speed) {
    OnForSyncEx(_outputs, _time, _speed, 0, true);
}

void OnForEx(uint8_t Outputs, int Time, int8_t Power, uint8_t reset);

static inline void OnFor(uint8_t _outputs, int _time) {
    OnForEx(_outputs, _time, OUT_POWER_DEFAULT, RESET_NONE);
}

void ResetTachoCount(uint8_t Outputs);

void ResetBlockTachoCount(uint8_t Outputs);

void ResetRotationCount(uint8_t Outputs);

void ResetAllTachoCounts(uint8_t Outputs);

void ResetCount(uint8_t Outputs, uint8_t reset);

int MotorTachoCount(uint8_t Output);

int MotorBlockTachoCount(uint8_t Output);

int8_t MotorPower(uint8_t Output);

int8_t MotorActualSpeed(uint8_t Output);

int MotorRotationCount(uint8_t Output);

#endif

#ifdef __cplusplus
}
#endif
