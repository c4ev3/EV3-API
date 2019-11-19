#ifdef __cplusplus
extern "C" {
#endif

#ifndef EV3_OUTPUT_PRIVATE_H
#define EV3_OUTPUT_PRIVATE_H

#include "../include/ev3_output.h"

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

#define On(_outputs) OnEx((_outputs), RESET_NONE)

#define Off(_outputs) OffEx((_outputs), RESET_NONE)

#define Float(_outputs) FloatEx((_outputs), RESET_NONE)

#define Coast(_outputs) FloatEx((_outputs), RESET_NONE)

void Toggle(uint8_t Outputs);

void Fwd(uint8_t Outputs);

void Rev(uint8_t Outputs);

bool MotorBusy(uint8_t Output);

#endif // ev3_output_h

#ifdef __cplusplus
}
#endif
