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

#include "ev3_constants.h"

bool OutputInit(void);

bool OutputOpen(void);

bool OutputClose(void);

bool OutputExit(void);

bool OutputProgramStop(void);

bool OutputInitialized(void);

// Stop outputs (brake or coast)
bool OutputStop(byte Outputs, bool useBrake);

// Set output device type
bool OutputSetType(byte Output, char DeviceType);

bool OutputSetTypesArray(char *pTypes);

bool OutputSetTypes(char OutputA, char OutputB, char OutputC, char OutputD);

// Reset position  (POS=0)
bool OutputReset(byte Outputs);

// Set speed (relative to polarity - enables regulation if the output has a tachometer)
bool OutputSpeed(byte Outputs, char Speed);

// Set power (suspend regulation and positioning)
bool OutputPower(byte Outputs, char Power);

// Starts outputs from present values
bool OutputStartEx(byte Outputs, byte Owner);

#define OutputStart(_outputs) OutputStartEx((_outputs), OWNER_NONE)

// Set polarity (0=toggle)
bool OutputPolarity(byte Outputs, char Polarity);

// Read actual speed and steps from last reset
bool OutputRead(byte Output, char * Speed, int * TachoCount, int * TachoSensor);

//bool OutputReady(byte Outputs, byte * Busy, byte Owner);

// check whether any of the outputs are busy
bool OutputTest(byte Outputs, bool * isBusy);

// set bitmask of all the busy outputs
bool OutputState(byte Outputs, byte * State);

// Clears the tacho count used when in sensor mode
bool OutputClearCount(byte Outputs);

// Gets the tacho count used in sensor mode
bool OutputGetCount(byte Output, int * Tacho);

// Read steps from last reset
bool OutputGetTachoCount(byte Output, int * Tacho);

// Read actual speed
bool OutputGetActualSpeed(byte Output, char * Speed);

bool OutputStepPowerEx(byte Outputs, char Power, int Step1, int Step2, int Step3, bool useBrake, byte Owner);

#define OutputStepPower(_outputs, _power, _step1, _step2, _step3) OutputStepPowerEx((_outputs), (_power), (_step1), (_step2), (_step3), TRUE, OWNER_NONE)

bool OutputTimePowerEx(byte Outputs, char Power, int Time1, int Time2, int Time3, bool useBrake, byte Owner);

#define OutputTimePower(_outputs, _power, _time1, _time2, _time3) OutputTimePowerEx((_outputs), (_power), (_time1), (_time2), (_time3), TRUE, OWNER_NONE)

bool OutputStepSpeedEx(byte Outputs, char Speed, int Step1, int Step2, int Step3, bool useBrake, byte Owner);

#define OutputStepSpeed(_outputs, _speed, _step1, _step2, _step3) OutputStepSpeedEx((_outputs), (_speed), (_step1), (_step2), (_step3), TRUE, OWNER_NONE)

bool OutputTimeSpeedEx(byte Outputs, char Speed, int Time1, int Time2, int Time3, bool useBrake, byte Owner);

#define OutputTimeSpeed(_outputs, _speed, _time1, _time2, _time3) OutputTimeSpeedEx((_outputs), (_speed), (_time1), (_time2), (_time3), TRUE, OWNER_NONE)

bool OutputStepSyncEx(byte Outputs, char Speed, short Turn, int Step, bool useBrake, byte Owner);

#define OutputStepSync(_outputs, _speed, _turn, _step) OutputStepSyncEx((_outputs), (_speed), (_turn), (_step), TRUE, OWNER_NONE)

bool OutputTimeSyncEx(byte Outputs, char Speed, short Turn, int Time, bool useBrake, byte Owner);

#define OutputTimeSync(_outputs, _speed, _turn, _time) OutputTimeSyncEx((_outputs), (_speed), (_turn), (_time), TRUE, OWNER_NONE)

void SetOutputEx(byte Outputs, byte Mode, byte reset);

#define SetOutput(_outputs, _mode) SetOutputEx((_outputs), (_mode), RESET_NONE)

void SetDirection(byte Outputs, byte Dir);

void SetPower(byte Outputs, char Power);

void SetSpeed(byte Outputs, char Speed);

void OnEx(byte Outputs, byte reset);

void OffEx(byte Outputs, byte reset);

void FloatEx(byte Outputs, byte reset);

#define On(_outputs) OnEx((_outputs), RESET_NONE)

#define Off(_outputs) OffEx((_outputs), RESET_NONE)

#define Float(_outputs) FloatEx((_outputs), RESET_NONE)

#define Coast(_outputs) FloatEx((_outputs), RESET_NONE)

void Toggle(byte Outputs);

void Fwd(byte Outputs);

void Rev(byte Outputs);

void OnFwdEx(byte Outputs, char Power, byte reset);

void OnRevEx(byte Outputs, char Power, byte reset);

#define OnFwd(_outputs) OnFwdEx((_outputs), OUT_POWER_DEFAULT, RESET_NONE)

#define OnRev(_outputs) OnRevEx((_outputs), OUT_POWER_DEFAULT, RESET_NONE)

void OnFwdRegEx(byte Outputs, char Speed, byte RegMode, byte reset);

void OnRevRegEx(byte Outputs, char Speed, byte RegMode, byte reset);

#define OnFwdReg(_outputs, _speed) OnFwdRegEx((_outputs), (_speed), OUT_REGMODE_SPEED, RESET_NONE)

#define OnRevReg(_outputs, _speed) OnRevRegEx((_outputs), (_speed), OUT_REGMODE_SPEED, RESET_NONE)

void OnFwdSyncEx(byte Outputs, char Speed, short Turn, byte reset);

void OnRevSyncEx(byte Outputs, char Speed, short Turn, byte reset);

#define OnFwdSync(_outputs, _speed) OnFwdSyncEx((_outputs), (_speed), 0, RESET_NONE)

#define OnRevSync(_outputs, _speed) OnRevSyncEx((_outputs), (_speed), 0, RESET_NONE)

void RotateMotorNoWaitEx(byte Outputs, char Speed, int Angle, short Turn, bool Sync, bool Stop);

#define RotateMotorNoWait(_outputs, _speed, _angle) RotateMotorNoWaitEx((_outputs), (_speed), (_angle), 0, TRUE, TRUE)

void RotateMotorEx(byte Outputs, char Speed, int Angle, short Turn, bool Sync, bool Stop);

#define RotateMotor(_outputs, _speed, _angle) RotateMotorEx((_outputs), (_speed), (_angle), 0, TRUE, TRUE)

void OnForSyncEx(byte Outputs, int Time, char Speed, short Turn, bool Stop);

#define OnForSync(_outputs, _time, _speed) OnForSyncEx((_outputs), (_time), (_speed), 0, TRUE)

void OnForEx(byte Outputs, int Time, char Power, byte reset);

#define OnFor(_outputs, _time) OnForEx((_outputs), (_time), OUT_POWER_DEFAULT, RESET_NONE)

void ResetTachoCount(byte Outputs);

void ResetBlockTachoCount(byte Outputs);

void ResetRotationCount(byte Outputs);

void ResetAllTachoCounts(byte Outputs);

void ResetCount(byte Outputs, byte reset);

int MotorTachoCount(byte Output);

int MotorBlockTachoCount(byte Output);

char MotorPower(byte Output);

char MotorActualSpeed(byte Output);

int MotorRotationCount(byte Output);

bool MotorBusy(byte Output);


//byte MotorGetBusyFlags(void);
//void MotorSetBusyFlags(byte Flags);
//void ResetDelayCounter(byte Pattern);

#endif // ev3_output_h

#ifdef __cplusplus
}
#endif
