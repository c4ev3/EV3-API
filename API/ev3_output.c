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
 */

#include "ev3_output.h"

static int __RAMP_UP_PCT = 10;
static int __RAMP_UP_DEGREES = 0;
static int __RAMP_DOWN_PCT = 10;
static int __RAMP_DOWN_DEGREES = 0;

typedef struct
{
  int TachoCounts;
  char Speed;
  int TachoSensor;
} MOTORDATA;

typedef struct
{
  byte Cmd;
  byte Outputs;
  char PwrOrSpd;
  int  StepOrTime1;
  int  StepOrTime2;
  int  StepOrTime3;
  byte Brake;
} StepOrTimePwrOrSpd;

typedef struct
{
  byte  Cmd;
  byte  Outputs;
  char  Speed;
  short Turn;
  int   StepOrTime;
  byte  Brake;
} StepOrTimeSync;

typedef struct
{
  char  OutputTypes[NUM_OUTPUTS];
  short Owners[NUM_OUTPUTS];

  int PwmFile;
  int MotorFile;

  MOTORDATA MotorData[NUM_OUTPUTS];
  MOTORDATA *pMotor;
} OutputGlobals;

OutputGlobals OutputInstance;

byte OutputToMotorNum(byte Output)
{
  switch(Output)
  {
    case OUT_A:
      return 0;
    case OUT_B:
      return 1;
    case OUT_C:
      return 2;
    case OUT_D:
      return 3;
  }
  return NUM_OUTPUTS;
}

void DecodeOutputs(byte * outputs, byte * layer)
{
  *layer   = *outputs & LAYER_MASK;
  *outputs = *outputs & OUT_MASK;
}

int WriteToPWMDevice(char * bytes, int num_bytes)
{
  int result = -1;
  if (OutputInstance.PwmFile >= 0)
  {
    // for some reason write is not returning num_bytes -
    // it usually returns zero
    result = write(OutputInstance.PwmFile, bytes, num_bytes);
    if (result >= 0)
      return num_bytes;
  }
  return result;
}

bool ResetOutputs(void)
{
  // stop all the motors
  int i;
  for (i=0; i < NUM_OUTPUTS; i++)
  {
    OutputInstance.Owners[i] = OWNER_NONE;
  }

  return OutputStop(OUT_ALL, false);
}

bool OutputInitialized(void)
{
  return (OutputInstance.PwmFile != -1) &&
            (OutputInstance.pMotor != NULL);
}

bool OutputInit(void)
{
  if (OutputInitialized())
    return true;

  MOTORDATA * pTmp;

  // To ensure that pMotor is never uninitialised
  OutputInstance.pMotor = OutputInstance.MotorData;

  // Open the handle for writing commands
  OutputInstance.PwmFile = open(LMS_PWM_DEVICE_NAME, O_RDWR);

  if (OutputInstance.PwmFile >= 0)
  {
    // Open the handle for reading motor values - shared memory
    OutputInstance.MotorFile = open(LMS_MOTOR_DEVICE_NAME, O_RDWR | O_SYNC);
    if (OutputInstance.MotorFile >= 0)
    {
      pTmp = (MOTORDATA*)mmap(0, sizeof(OutputInstance.MotorData), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, OutputInstance.MotorFile, 0);
      if (pTmp == MAP_FAILED)
      {
//        LogErrorNumber(OUTPUT_SHARED_MEMORY);
        close(OutputInstance.MotorFile);
        close(OutputInstance.PwmFile);
        OutputInstance.MotorFile = -1;
        OutputInstance.PwmFile = -1;
      }
      else
      {
        OutputInstance.pMotor = pTmp;
        return OutputOpen();
      }
    }
  }
  return false;
}

bool OutputOpen(void)
{
  if (!OutputInitialized())
    return false;

  char cmd;

  bool result = ResetOutputs();
  if (result)
  {
    cmd = opProgramStart;
    return WriteToPWMDevice(&cmd, 1) == 1;
  }

  return result;
}

bool OutputClose(void)
{
  if (!OutputInitialized())
    return false;

  return ResetOutputs();
}


bool OutputExit(void)
{
  if (!OutputInitialized())
    return true;

  // otherwise, close down the output module

  bool result = ResetOutputs();
  if (OutputInstance.MotorFile >= 0)
  {
    munmap(OutputInstance.pMotor, sizeof(OutputInstance.MotorData));
    close(OutputInstance.MotorFile);
    OutputInstance.MotorFile = -1;
  }
  if (OutputInstance.PwmFile >= 0)
  {
    close(OutputInstance.PwmFile);
    OutputInstance.PwmFile = -1;
  }
  return result;
}


bool OutputStop(byte Outputs, bool useBrake)
{
  if (!OutputInitialized())
    return false;
  int cmdLen = 3;
  char cmd[3];
  byte Layer;
  // opOutputStop (outputs, brake)
  // Stops the outputs (brake or coast)
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputStop;
    cmd[1] = Outputs;
    cmd[2] = useBrake;
    return WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  }
  else
  {
    return false;
    // support for daisychaining not yet implemented
/*

      if (cDaisyReady() != BUSY)

      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STOP;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputProgramStop(void)
{
  if (!OutputInitialized())
    return false;

  char cmd;
  cmd = opProgramStop;
  return WriteToPWMDevice(&cmd, 1) == 1;
}

bool OutputSetType(byte Output, char DeviceType)
{
  if (!OutputInitialized())
    return false;

  byte Layer;
  // opOutputSetType (output, type)  </b>
  // Set output device type
  DecodeOutputs(&Output, &Layer);
  if (Layer == LAYER_MASTER)
  {
    Output = OutputToMotorNum(Output);
    if (Output < NUM_OUTPUTS)
    {
      if (OutputInstance.OutputTypes[Output] != DeviceType)
      {
        OutputInstance.OutputTypes[Output] = DeviceType;
        // shouldn't this also write the information to the device?
        return OutputSetTypesArray(OutputInstance.OutputTypes);
      }
    }
    return false;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_RESET;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)No, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Type, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputSetTypesArray(char * pTypes)
{
  return OutputSetTypes(pTypes[0], pTypes[1], pTypes[2], pTypes[3]);
}

bool OutputSetTypes(char OutputA, char OutputB, char OutputC, char OutputD)
{
  if (!OutputInitialized())
    return false;
  int cmdLen = 5;
  char cmd[5];
  cmd[0] = opOutputSetType;
  cmd[1] = OutputA;
  cmd[2] = OutputB;
  cmd[3] = OutputC;
  cmd[4] = OutputD;
  bool result = WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  if (result)
  {
    int i;
    for (i = 0; i < NUM_OUTPUTS; i++)
    {
      if (OutputInstance.OutputTypes[i] != cmd[i+1])
        OutputInstance.OutputTypes[i] = cmd[i+1];
    }
  }
  return result;
}

bool OutputReset(byte Outputs)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = 2;
  byte Layer;
  char cmd[2];
  // opOutputReset (outputs)
  // Resets the Tacho counts
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputReset;
    cmd[1] = Outputs;
    return WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_RESET;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputSpeed(byte Outputs, char Speed)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = 3;
  byte Layer;
  char cmd[3];
  // opOutputSpeed (outputs, speed)
  // Set speed of the outputs
  // (relative to polarity - enables regulation if the output has a tachometer)
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputSpeed;
    cmd[1] = Outputs;
    cmd[2] = Speed;
    return WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Speed, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputPower(byte Outputs, char Power)
{
  if (!OutputInitialized())
    return false;
  int cmdLen = 3;
  byte Layer;
  char cmd[3];
  // opOutputPower (outputs, power)
  // Set power of the outputs
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputPower;
    cmd[1] = Outputs;
    cmd[2] = Power;
    return WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Power, &(DaisyBuf[Len]));

        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
          // printf("cOutPut @ opOUTPUT_POWER after cDaisyDownStreamCmd - OK and WriteState = %d\n\r", cDaisyGetLastWriteState());
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputStartEx(byte Outputs, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = 2;
  byte Layer;
  char cmd[2];
  // opOutputStart (outputs)
  // Starts the outputs
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputStart;
    cmd[1] = Outputs;
    bool result = WriteToPWMDevice(cmd, cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_START;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Nos, &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
          //printf("cOutPut @ opOUTPUT_START after cDaisyDownStreamCmd - OK and WriteState = %d\n\r", cDaisyGetLastWriteState());
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);

      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputPolarity(byte Outputs, char Polarity)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = 3;
  byte Layer;
  char cmd[3];
  // opOutputPolarity (outputs, polarity)
  // Set polarity of the outputs
  //  - -1 makes the motor run backward
  //  -  1 makes the motor run forward
  //  -  0 makes the motor run the opposite direction
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputPolarity;
    cmd[1] = Outputs;
    cmd[2] = Polarity;
    return WriteToPWMDevice(cmd, cmdLen) == cmdLen;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_POLARITY;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Polarity[1], &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)Polarity[2], &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputStepPowerEx(byte Outputs, char Power, int Step1, int Step2, int Step3, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimePwrOrSpd);
  byte Layer;
  StepOrTimePwrOrSpd cmd;
  // opOutputStepPower (outputs, power, step1, step2, step3, brake?)
  // Set Ramp up, constant and rampdown steps and power of the outputs
  DecodeOutputs(&Outputs, &Layer);
  cmd.Cmd = opOutputStepPower;
  cmd.Outputs = Outputs;
  cmd.PwrOrSpd = Power;
  cmd.StepOrTime1 = Step1;
  cmd.StepOrTime2 = Step2;
  cmd.StepOrTime3 = Step3;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Power, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Step3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepPower.Brake, &(DaisyBuf[Len]));

        //if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepPower.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }

        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepPower.Nos);

      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputTimePowerEx(byte Outputs, char Power, int Time1, int Time2, int Time3, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimePwrOrSpd);
  byte Layer;
  StepOrTimePwrOrSpd cmd;
  // opOutputTimePower (outputs, power, time1, time2, time3, brake?)
  // Set Ramp up, constant and rampdown steps and power of the outputs
  DecodeOutputs(&Outputs, &Layer);
  cmd.Cmd = opOutputTimePower;
  cmd.Outputs = Outputs;
  cmd.PwrOrSpd = Power;
  cmd.StepOrTime1 = Time1;
  cmd.StepOrTime2 = Time2;
  cmd.StepOrTime3 = Time3;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_POWER;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Power, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Time3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimePower.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimePower.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimePower.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputStepSpeedEx(byte Outputs, char Speed, int Step1, int Step2, int Step3, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimePwrOrSpd);
  byte Layer;
  StepOrTimePwrOrSpd cmd;
  // opOutputStepSpeed (outputs, power, step1, step2, step3, brake?)
  // Set Ramp up, constant and rampdown steps and speed of the outputs
  DecodeOutputs(&Outputs, &Layer);
  cmd.Cmd = opOutputStepSpeed;
  cmd.Outputs = Outputs;
  cmd.PwrOrSpd = Speed;
  cmd.StepOrTime1 = Step1;
  cmd.StepOrTime2 = Step2;
  cmd.StepOrTime3 = Step3;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Step3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSpeed.Brake, &(DaisyBuf[Len]));

        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSpeed.Nos))
        {
          printf("NOT ok txed cOutputStepSpeed\n\r");
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        else
        {
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSpeed.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputTimeSpeedEx(byte Outputs, char Speed, int Time1, int Time2, int Time3, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimePwrOrSpd);
  byte Layer;
  StepOrTimePwrOrSpd cmd;
  // opOutputTimeSpeed (outputs, speed, time1, time2, time3, brake?)
  // Set Ramp up, constant and rampdown steps and power of the outputs
  DecodeOutputs(&Outputs, &Layer);
  cmd.Cmd = opOutputTimeSpeed;
  cmd.Outputs = Outputs;
  cmd.PwrOrSpd = Speed;
  cmd.StepOrTime1 = Time1;
  cmd.StepOrTime2 = Time2;
  cmd.StepOrTime3 = Time3;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_SPEED;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time1, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time2, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Time3, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSpeed.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSpeed.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSpeed.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/

  }

}

bool OutputStepSyncEx(byte Outputs, char Speed, short Turn, int Step, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimeSync);
  byte Layer;
  StepOrTimeSync cmd;
  // opOutputStepSync (outputs, speed, turn, step, brake?)
  DecodeOutputs(&Outputs, &Layer);
  // it is invalid to call this function with anything other than 2 motors:
  // i.e., OUT_AB, OUT_AC, OUT_AD, OUT_BC, OUT_BD, or OUT_CD
  if (!(Outputs == OUT_AB || Outputs == OUT_AC || Outputs == OUT_AD ||
        Outputs == OUT_BC || Outputs == OUT_BD || Outputs == OUT_CD))
    return false;
  cmd.Cmd = opOutputStepSync;
  cmd.Outputs = Outputs;
  cmd.Speed = Speed;
  cmd.Turn = Turn;
  cmd.StepOrTime = Step;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_STEP_SYNC;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Turn,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Step,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)StepSync.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSync.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, StepSync.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputTimeSyncEx(byte Outputs, char Speed, short Turn, int Time, bool useBrake, byte Owner)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = sizeof(StepOrTimeSync);
  byte Layer;
  StepOrTimeSync cmd;
  // opOutputTimeSync (outputs, speed, turn, time, brake?)
  DecodeOutputs(&Outputs, &Layer);
  // it is invalid to call this function with anything other than 2 motors:
  // i.e., OUT_AB, OUT_AC, OUT_AD, OUT_BC, OUT_BD, or OUT_CD
  if (!(Outputs == OUT_AB || Outputs == OUT_AC || Outputs == OUT_AD ||
        Outputs == OUT_BC || Outputs == OUT_BD || Outputs == OUT_CD))
    return false;
  cmd.Cmd = opOutputTimeSync;
  cmd.Outputs = Outputs;
  cmd.Speed = Speed;
  cmd.Turn = Turn;
  cmd.StepOrTime = Time;
  cmd.Brake = (byte)useBrake;
  if (Layer == LAYER_MASTER)
  {
    bool result = WriteToPWMDevice((char*)&(cmd.Cmd), cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.Owners[i] = Owner;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_TIME_SYNC;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Nos,   &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Speed, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Turn,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Time,  &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)TimeSync.Brake, &(DaisyBuf[Len]));
        if(OK != cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSync.Nos))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyMotorDownStream(DaisyBuf, Len, Layer, TimeSync.Nos);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputRead(byte Output, char * Speed, int * TachoCount, int * TachoSensor)
{
  if (!OutputInitialized())
    return false;

  byte Layer;
  // opOutputRead (output, *speed, *tachocount, *tachosensor)
  // Speed [-100..100]
  // Tacho count [-MAX .. +MAX]
  // Tacho sensor [-MAX .. +MAX]
  *Speed = 0;
  *TachoCount = 0;
  *TachoSensor = 0;
  DecodeOutputs(&Output, &Layer);
  if (Layer == LAYER_MASTER)
  {
    Output = OutputToMotorNum(Output);
    if (Output < NUM_OUTPUTS)
    {
      *Speed = OutputInstance.pMotor[Output].Speed;
      *TachoCount = OutputInstance.pMotor[Output].TachoCounts;
      *TachoSensor = OutputInstance.pMotor[Output].TachoSensor;
      return true;
    }
  }
  return false;
  // the firmware code does not contain any sign of letting you read
  // tacho values from other layers via this opcode.
}

/*
// the firmware implementation of this call is a blocking
// call that won't let a program continue executing the current object
// until the output(s) are ready.
bool OutputReady(byte Outputs, byte * Busy, byte Owner)
{
  if (!OutputInitialized())
    return false;

  bool Result = true;
  *Busy = 0;
  int test = 0;
  int test2 = 0;
  char busyReturn[10];
  byte Layer;
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {

    if (OutputInstance.PwmFile >= 0)

    {

      read(OutputInstance.PwmFile, busyReturn, 4);

      sscanf(busyReturn,"%u %u",&test,&test2);

      int Tmp;

      for (Tmp = 0; Tmp < NUM_OUTPUTS - 1; Tmp++)

      {

        // Check resources for NOTREADY

        if (Outputs & (0x01 << Tmp))

        {

          // Only relevant ones

          if (test & (0x01 << Tmp))

          {

            // If BUSY check for OVERRULED

            if (OutputInstance.Owners[Tmp] == Owner)

              break;

          }

        }

      }

    }

  }

  else

  {

//    Bits = cDaisyCheckBusyBit(Layer, Nos);

//    Bits = 0;
//
//    for (Tmp = 0;(Tmp < OUTPUTS) && (DspStat == NOBREAK);Tmp++)
//    {
//      // Check resources for NOTREADY
//      if (Nos & (1 << Tmp))
//      {
//        // Only relevant ones
//        if (Bits & (1 << Tmp))
//        {
//          // If BUSY check for OVERRULED
//          if (OutputInstance.Owners[Tmp] == Owner)
//          {
//            DspStat  =  BUSYBREAK;
//          }
//        }
//      }
//    }
  }

}
*/

bool OutputTest(byte Outputs, bool * isBusy)
{
  if (!OutputInitialized())
    return false;

  byte Layer;
  int  test;
  int  test2;
  char busyReturn[20]; // Busy mask
  bool result = false;

  *isBusy = false;
  test2 = 0;
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      size_t bytes_read = read(OutputInstance.PwmFile, busyReturn, 10);
      result = bytes_read > 0;
      sscanf(busyReturn, "%u %u", &test, &test2);
      *isBusy = ((Outputs & (byte)test2) != 0);
    }
  }
  else
  {
//    if cDaisyCheckBusyBit(Layer, Outputs) )
      *isBusy = false;
  }
  return result;
}

bool OutputState(byte Outputs, byte * State)
{
  if (!OutputInitialized())
    return false;

  byte Layer;
  int  test;
  int  test2;
  char busyReturn[20]; // Busy mask
  bool result = false;

  *State = 0;
  test2 = 0;
  test = 0;
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    if (OutputInstance.PwmFile >= 0)
    {
      size_t bytes_read = read(OutputInstance.PwmFile, busyReturn, 10);
      result = bytes_read > 0;
      sscanf(busyReturn, "%u %u", &test, &test2);
      *State = test2;
    }
  }
  else
  {
//    if cDaisyCheckBusyBit(Layer, Outputs) )
      *State = 0;
  }
  return result;
}

bool OutputClearCount(byte Outputs)
{
  if (!OutputInitialized())
    return false;

  int cmdLen = 2;
  byte Layer;
  char cmd[2];
  DecodeOutputs(&Outputs, &Layer);
  if (Layer == LAYER_MASTER)
  {
    cmd[0] = opOutputClearCount;
    cmd[1] = Outputs;
    bool result = WriteToPWMDevice(cmd, cmdLen) == cmdLen;
    if (result)
    {
      int i;
      for (i = 0; i < NUM_OUTPUTS; i++)
      {
        if (Outputs & (0x01 << i))
          OutputInstance.pMotor[i].TachoSensor = 0;
      }
    }
    return result;
  }
  else
  {
    return false;
/*
      if (cDaisyReady() != BUSY)
      {
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  0;
        DaisyBuf[Len++]  =  opOUTPUT_CLR_COUNT;
        Len             +=  cOutputPackParam((DATA32)0, &(DaisyBuf[Len]));
        Len             +=  cOutputPackParam((DATA32)ClrCnt[1], &(DaisyBuf[Len]));
        if(OK != cDaisyDownStreamCmd(DaisyBuf, Len, Layer))
        {
          SetObjectIp(TmpIp - 1);
          DspStat  =  BUSYBREAK;
        }
        //cDaisyDownStreamCmd(DaisyBuf, Len, Layer);
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        DspStat  =  BUSYBREAK;
      }
*/
  }
}

bool OutputGetCount(byte Output, int * Tacho)
{
  char speed = 0;
  int tcount = 0;
  return OutputRead(Output, &speed, &tcount, Tacho);
}

bool OutputGetTachoCount(byte Output, int * Tacho)
{
  char speed = 0;
  int tsensor = 0;
  return OutputRead(Output, &speed, Tacho, &tsensor);
}

bool OutputGetActualSpeed(byte Output, char * Speed)
{
  int tcount = 0;
  int tsensor = 0;
  return OutputRead(Output, Speed, &tcount, &tsensor);
}

/*
uint DELAY_COUNTER = 0;
UBYTE	BusyOnes = 0;

void ResetDelayCounter(UBYTE Pattern)
{
	BusyOnes = Pattern;
	DELAY_COUNTER = 0;
}

UBYTE     cMotorGetBusyFlags(void)
{
  int     test, test2;
  char    BusyReturn[10]; // Busy mask

  if (OutputInstance.PwmFile >= 0)
  {
    read(OutputInstance.PwmFile,BusyReturn,4);
    sscanf(BusyReturn,"%u %u",&test,&test2);
  }
  else
  {
    test = 0;
  }
  if(DELAY_COUNTER < 25)
  {
	  test = BusyOnes;
	  DELAY_COUNTER++;
  }

  return(test);
}


void      cMotorSetBusyFlags(UBYTE Flags)
{
  if (OutputInstance.MotorFile >= 0)
  {
    write(OutputInstance.MotorFile, &Flags, sizeof(Flags));
  }
}
*/

void SetOutputEx(byte Outputs, byte Mode, byte reset)
{
  switch (Mode)
  {
    case OUT_FLOAT :
      OutputStop(Outputs, false);
      ResetCount(Outputs, reset);
      break;
    case OUT_OFF :
      OutputStop(Outputs, true);
      ResetCount(Outputs, reset);
      break;
    case OUT_ON :
      ResetCount(Outputs, reset);
      OutputStart(Outputs);
      break;
  }
}

void SetDirection(byte Outputs, byte Dir)
{
  char Polarity;
  switch (Dir)
  {
    case OUT_REV :
      Polarity = -1;
      break;
    case OUT_TOGGLE :
      Polarity = 0;
      break;
    default:
      Polarity = 1;
      break;
  }
  OutputPolarity(Outputs, Polarity);
}

void SetPower(byte Outputs, char Power)
{
  OutputPower(Outputs, Power);
}

void SetSpeed(byte Outputs, char Speed)
{
  OutputSpeed(Outputs, Speed);
}

void OnEx(byte Outputs, byte reset)
{
  SetOutputEx(Outputs, OUT_ON, reset);
}

void OffEx(byte Outputs, byte reset)
{
  SetOutputEx(Outputs, OUT_OFF, reset);
}

void FloatEx(byte Outputs, byte reset)
{
  SetOutputEx(Outputs, OUT_FLOAT, reset);
}

void Toggle(byte Outputs)
{
  SetDirection(Outputs, OUT_TOGGLE);
}

void Fwd(byte Outputs)
{
  SetDirection(Outputs, OUT_FWD);
}

void Rev(byte Outputs)
{
  SetDirection(Outputs, OUT_REV);
}

void OnFwdEx(byte Outputs, char Power, byte reset)
{
  Fwd(Outputs);
  if (Power != OUT_POWER_DEFAULT)
    SetPower(Outputs, Power);
  OnEx(Outputs, reset);
}

void OnRevEx(byte Outputs, char Power, byte reset)
{
  Rev(Outputs);
  if (Power != OUT_POWER_DEFAULT)
    SetPower(Outputs, Power);
  OnEx(Outputs, reset);
}

void OnFwdRegEx(byte Outputs, char Speed, byte RegMode, byte reset)
{
  // regmode parameter is ignored
  Fwd(Outputs);
  SetSpeed(Outputs, Speed);
  OnEx(Outputs, reset);
}

void OnRevRegEx(byte Outputs, char Speed, byte RegMode, byte reset)
{
  // regmode parameter is ignored
  Rev(Outputs);
  SetSpeed(Outputs, Speed);
  OnEx(Outputs, reset);
}

void OnFwdSyncEx(byte Outputs, char Speed, short Turn, byte reset)
{
  ResetCount(Outputs, reset);
  OutputStepSyncEx(Outputs, Speed, Turn, INT_MAX, FALSE, OWNER_NONE);
}

void OnRevSyncEx(byte Outputs, char Speed, short Turn, byte reset)
{
  OnFwdSyncEx(Outputs, Speed*-1, Turn, reset);
}

#define MIN(a,b) (((a)<(b))?(a):(b))

void RotateMotorNoWaitEx(byte Outputs, char Speed, int Angle, short Turn, bool Sync, bool Stop)
{
  if (Sync)
  {
    byte Layer, tmpOuts;
    tmpOuts = Outputs;
    DecodeOutputs(&tmpOuts, &Layer);
    if (tmpOuts == OUT_AB || tmpOuts == OUT_AC || tmpOuts == OUT_AD ||
        tmpOuts == OUT_BC || tmpOuts == OUT_BD || tmpOuts == OUT_CD)
    {
      OutputStepSyncEx(Outputs, Speed, Turn, Angle, Stop, OWNER_NONE);
      return;
    }
  }
  // otherwise use a non-synchronized API call
  int s3 = MIN(Angle / __RAMP_DOWN_PCT, __RAMP_DOWN_DEGREES), s1 = MIN(Angle / __RAMP_UP_PCT, __RAMP_UP_DEGREES), s2 = Angle - s3 - s1;
  OutputStepSpeedEx(Outputs, Speed, s1, s2, s3, Stop, OWNER_NONE);
}

void RotateMotorEx(byte Outputs, char Speed, int Angle, short Turn, bool Sync, bool Stop)
{
  RotateMotorNoWaitEx(Outputs, Speed, Angle, Turn, Sync, Stop);
  bool busy;
  while (true)
  {
    Wait(MS_2); // 2ms between checks
    busy = false;
    OutputTest(Outputs, &busy);
    if (!busy) break;
  }
}

void OnForSyncEx(byte Outputs, int Time, char Speed, short Turn, bool Stop)
{
  OutputTimeSyncEx(Outputs, Speed, Turn, Time, Stop, OWNER_NONE);
}

void OnForEx(byte Outputs, int Time, char Power, byte reset)
{
  if (Power != OUT_POWER_DEFAULT)
    SetPower(Outputs, Power);
  OnEx(Outputs, reset);
  usleep(Time);
  OffEx(Outputs, reset);
}

void ResetTachoCount(byte Outputs)
{
  // reset tacho counter(s)
  OutputReset(Outputs);
}

void ResetBlockTachoCount(byte Outputs)
{
  // synonym for ResetTachoCount
  ResetTachoCount(Outputs);
}

void ResetRotationCount(byte Outputs)
{
  // reset tacho counter(s)
  OutputClearCount(Outputs);
}

void ResetAllTachoCounts(byte Outputs)
{
  // clear all tacho counts
  OutputReset(Outputs);
  OutputClearCount(Outputs);
}

void ResetBlockAndTachoCount(byte Outputs)
{
  // synonym for ResetTachoCount
  ResetTachoCount(Outputs);
}

void ResetCount(byte Outputs, byte reset)
{
  // reset tacho counter(s)
  switch (reset)
  {
    case RESET_COUNT :
      ResetTachoCount(Outputs);
      return;
    case RESET_BLOCK_COUNT :
      ResetBlockTachoCount(Outputs);
      return;
    case RESET_ROTATION_COUNT :
      ResetRotationCount(Outputs);
      return;
    case RESET_BLOCKANDTACHO :
      ResetBlockAndTachoCount(Outputs);
      return;
    case RESET_ALL :
      ResetAllTachoCounts(Outputs);
      return;
  }
}

int MotorTachoCount(byte Output)
{
  int Result = 0;
  OutputGetTachoCount(Output, &Result);
  return Result;
}

int MotorBlockTachoCount(byte Output)
{
  int Result = 0;
  OutputGetTachoCount(Output, &Result);
  return Result;
}

char MotorPower(byte Output)
{
  char Result = 0;
  OutputGetActualSpeed(Output, &Result);
  return Result;
}

char MotorActualSpeed(byte Output)
{
  char Result = 0;
  OutputGetActualSpeed(Output, &Result);
  return Result;
}

int MotorRotationCount(byte Output)
{
  int Result = 0;
  OutputGetCount(Output, &Result);
  return Result;
}

bool MotorBusy(byte Output)
{
  bool Result = false;
  OutputTest(Output, &Result);
  return Result;
}

