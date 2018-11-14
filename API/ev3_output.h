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

#include "ev3_command.h"
#include "ev3_constants.h"

bool OutputInit(void);

bool OutputOpen(void);

bool OutputClose(void);

bool OutputExit(void);

bool OutputProgramStop(void);

bool OutputInitialized(void);

// Stop outputs (brake or coast)
bool OutputStop(uint8_t Outputs, bool useBrake);

// Set output device type
bool OutputSetType(uint8_t Output, int8_t DeviceType);

bool OutputSetTypesArray(int8_t *pTypes);

bool OutputSetTypes(int8_t OutputA, int8_t OutputB, int8_t OutputC, int8_t OutputD);

// Reset position  (POS=0)
bool OutputReset(uint8_t Outputs);

// Set speed (relative to polarity - enables regulation if the output has a tachometer)
bool OutputSpeed(uint8_t Outputs, int8_t Speed);

// Set power (suspend regulation and positioning)
bool OutputPower(uint8_t Outputs, int8_t Power);

// Starts outputs from present values
bool OutputStartEx(uint8_t Outputs, uint8_t Owner);

#define OutputStart(_outputs) OutputStartEx((_outputs), OWNER_NONE)

// Set polarity (0=toggle)
bool OutputPolarity(uint8_t Outputs, int8_t Polarity);

// Read actual speed and steps from last reset
bool OutputRead(uint8_t Output, int8_t * Speed, int * TachoCount, int * TachoSensor);

//bool OutputReady(uint8_t Outputs, uint8_t * Busy, uint8_t Owner);

// check whether any of the outputs are busy
bool OutputTest(uint8_t Outputs, bool * isBusy);

// set bitmask of all the busy outputs
bool OutputState(uint8_t Outputs, uint8_t * State);

// Clears the tacho count used when in sensor mode
bool OutputClearCount(uint8_t Outputs);

// Gets the tacho count used in sensor mode
bool OutputGetCount(uint8_t Output, int * Tacho);

// Read steps from last reset
bool OutputGetTachoCount(uint8_t Output, int * Tacho);

// Read actual speed
bool OutputGetActualSpeed(uint8_t Output, int8_t * Speed);

bool OutputStepPowerEx(uint8_t Outputs, int8_t Power, int Step1, int Step2, int Step3, bool useBrake, uint8_t Owner);

#define OutputStepPower(_outputs, _power, _step1, _step2, _step3) OutputStepPowerEx((_outputs), (_power), (_step1), (_step2), (_step3), true, OWNER_NONE)

bool OutputTimePowerEx(uint8_t Outputs, int8_t Power, int Time1, int Time2, int Time3, bool useBrake, uint8_t Owner);

#define OutputTimePower(_outputs, _power, _time1, _time2, _time3) OutputTimePowerEx((_outputs), (_power), (_time1), (_time2), (_time3), true, OWNER_NONE)

bool OutputStepSpeedEx(uint8_t Outputs, int8_t Speed, int Step1, int Step2, int Step3, bool useBrake, uint8_t Owner);

#define OutputStepSpeed(_outputs, _speed, _step1, _step2, _step3) OutputStepSpeedEx((_outputs), (_speed), (_step1), (_step2), (_step3), true, OWNER_NONE)

bool OutputTimeSpeedEx(uint8_t Outputs, int8_t Speed, int Time1, int Time2, int Time3, bool useBrake, uint8_t Owner);

#define OutputTimeSpeed(_outputs, _speed, _time1, _time2, _time3) OutputTimeSpeedEx((_outputs), (_speed), (_time1), (_time2), (_time3), true, OWNER_NONE)

bool OutputStepSyncEx(uint8_t Outputs, int8_t Speed, short Turn, int Step, bool useBrake, uint8_t Owner);

#define OutputStepSync(_outputs, _speed, _turn, _step) OutputStepSyncEx((_outputs), (_speed), (_turn), (_step), true, OWNER_NONE)

bool OutputTimeSyncEx(uint8_t Outputs, int8_t Speed, short Turn, int Time, bool useBrake, uint8_t Owner);

#define OutputTimeSync(_outputs, _speed, _turn, _time) OutputTimeSyncEx((_outputs), (_speed), (_turn), (_time), true, OWNER_NONE)

void SetOutputEx(uint8_t Outputs, uint8_t Mode, uint8_t reset);

#define SetOutput(_outputs, _mode) SetOutputEx((_outputs), (_mode), RESET_NONE)

void SetDirection(uint8_t Outputs, uint8_t Dir);

void SetPower(uint8_t Outputs, int8_t Power);

void SetSpeed(uint8_t Outputs, int8_t Speed);

void OnEx(uint8_t Outputs, uint8_t reset);

void OffEx(uint8_t Outputs, uint8_t reset);

void FloatEx(uint8_t Outputs, uint8_t reset);

#define On(_outputs) OnEx((_outputs), RESET_NONE)

#define Off(_outputs) OffEx((_outputs), RESET_NONE)

#define Float(_outputs) FloatEx((_outputs), RESET_NONE)

#define Coast(_outputs) FloatEx((_outputs), RESET_NONE)

void Toggle(uint8_t Outputs);

void Fwd(uint8_t Outputs);

void Rev(uint8_t Outputs);

void OnFwdEx(uint8_t Outputs, int8_t Power, uint8_t reset);

void OnRevEx(uint8_t Outputs, int8_t Power, uint8_t reset);

#define OnFwd(_outputs) OnFwdEx((_outputs), OUT_POWER_DEFAULT, RESET_NONE)

#define OnRev(_outputs) OnRevEx((_outputs), OUT_POWER_DEFAULT, RESET_NONE)

void OnFwdRegEx(uint8_t Outputs, int8_t Speed, uint8_t RegMode, uint8_t reset);

void OnRevRegEx(uint8_t Outputs, int8_t Speed, uint8_t RegMode, uint8_t reset);

#define OnFwdReg(_outputs, _speed) OnFwdRegEx((_outputs), (_speed), OUT_REGMODE_SPEED, RESET_NONE)

#define OnRevReg(_outputs, _speed) OnRevRegEx((_outputs), (_speed), OUT_REGMODE_SPEED, RESET_NONE)

void OnFwdSyncEx(uint8_t Outputs, int8_t Speed, short Turn, uint8_t reset);

void OnRevSyncEx(uint8_t Outputs, int8_t Speed, short Turn, uint8_t reset);

#define OnFwdSync(_outputs, _speed) OnFwdSyncEx((_outputs), (_speed), 0, RESET_NONE)

#define OnRevSync(_outputs, _speed) OnRevSyncEx((_outputs), (_speed), 0, RESET_NONE)

void RotateMotorNoWaitEx(uint8_t Outputs, int8_t Speed, int Angle, short Turn, bool Sync, bool Stop);

#define RotateMotorNoWait(_outputs, _speed, _angle) RotateMotorNoWaitEx((_outputs), (_speed), (_angle), 0, true, true)

void RotateMotorEx(uint8_t Outputs, int8_t Speed, int Angle, short Turn, bool Sync, bool Stop);

#define RotateMotor(_outputs, _speed, _angle) RotateMotorEx((_outputs), (_speed), (_angle), 0, true, true)

void OnForSyncEx(uint8_t Outputs, int Time, int8_t Speed, short Turn, bool Stop);

#define OnForSync(_outputs, _time, _speed) OnForSyncEx((_outputs), (_time), (_speed), 0, true)

void OnForEx(uint8_t Outputs, int Time, int8_t Power, uint8_t reset);

#define OnFor(_outputs, _time) OnForEx((_outputs), (_time), OUT_POWER_DEFAULT, RESET_NONE)

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

bool MotorBusy(uint8_t Output);


//uint8_t MotorGetBusyFlags(void);
//void MotorSetBusyFlags(uint8_t Flags);
//void ResetDelayCounter(uint8_t Pattern);

#endif // ev3_output_h

#ifdef __cplusplus
}
#endif
