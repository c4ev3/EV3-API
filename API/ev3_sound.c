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

#include "ev3_sound.h"

#define STEP_SIZE_TABLE_ENTRIES 89
#define INDEX_TABLE_ENTRIES     16

// sound commands
#define SOUND_CMD_BREAK   0
#define SOUND_CMD_TONE    1
#define SOUND_CMD_PLAY    2
#define SOUND_CMD_REPEAT  3
#define SOUND_CMD_SERVICE 4

#define SOUND_MODE_ONCE           0x00
#define SOUND_LOOP                0x01
#define SOUND_ADPCM_INIT_VALPREV  0x7F
#define SOUND_ADPCM_INIT_INDEX    20
#define SOUND_MAX_DATA_SIZE       250
#define SOUND_CHUNK               250
#define SOUND_ADPCM_CHUNK         125
#define SOUND_MASTER_CLOCK        132000000
#define SOUND_TONE_MASTER_CLOCK   1031250
#define SOUND_MIN_FRQ             250
#define SOUND_MAX_FRQ             10000
#define SOUND_MAX_LEVEL           8
#define SOUND_FILE_BUFFER_SIZE    (SOUND_CHUNK + 2) //!< 12.8 mS @ 8KHz
#define SOUND_BUFFER_COUNT        3
#define SOUND_FILE_FORMAT_RAW_SOUND   0x0100 //!< RSO-file
#define SOUND_FILE_FORMAT_ADPCM_SOUND 0x0101 //!< RSO-file compressed

const short StepSizeTable[STEP_SIZE_TABLE_ENTRIES] = {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
        19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
        50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
        130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
        337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
        876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
        2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
        5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
        15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
    };

const short IndexTable[INDEX_TABLE_ENTRIES] = {
     -1, -1, -1, -1, 2, 4, 6, 8,
     -1, -1, -1, -1, 2, 4, 6, 8
   };

typedef struct {
  byte Busy;
} SOUND_BUSY;

typedef struct {
  int SoundDriverDescriptor;
  int hSoundFile;
  byte SoundOwner;
  byte SoundState;
  SOUND_BUSY Sound;
  SOUND_BUSY *pSound;
  byte SoundMuted;
  unsigned short BytesLeft;
  unsigned short SoundFileFormat;
  int SoundDataLength;
  unsigned short SoundSampleRate;
  unsigned short SoundPlayMode;
  int SoundFileLength;
  short ValPrev;
  short Index;
  short Step;
  byte BytesToWrite;
  byte StopLoop;
  char PathBuffer[FILENAME_SIZE];
  struct stat FileStatus;
  byte SoundData[SOUND_FILE_BUFFER_SIZE + 1];
} SoundGlobals;

SoundGlobals SoundInstance;

int WriteToSoundDevice(char * bytes, int num_bytes)
{
  ssize_t result = -1;
  int sndHandle = open(LMS_SOUND_DEVICE_NAME, O_WRONLY);
  if (sndHandle >= 0)
  {
    // for some reason write is not returning num_bytes -
    // it usually returns zero
    result = write(sndHandle, bytes, num_bytes);
//printf("result = %zd\n", result);
    close(sndHandle);
//    if (result >= 0)
//      return num_bytes;
  }
  return (int)result;
}

void SoundCloseDevices()
{
  if (!SoundInitialized())
    return;

  if ((SoundInstance.pSound != NULL) &&
      (SoundInstance.pSound != &SoundInstance.Sound))
  {
    munmap(SoundInstance.pSound, sizeof(unsigned short));
  }

  SoundInstance.pSound = NULL;
}

bool SoundInit()
{
  int sndHandle;
  bool Result = SoundInitialized();
  if (!Result)
  {
    SoundInstance.SoundDriverDescriptor = -1;
    SoundInstance.hSoundFile = -1;
    SoundInstance.pSound = &SoundInstance.Sound;

    // Create a Shared Memory entry for signaling the driver state BUSY or NOT BUSY
    
    sndHandle = open(LMS_SOUND_DEVICE_NAME, O_RDWR | O_SYNC);
    if (sndHandle != -1)
    {
      SOUND_BUSY * pTmp = (SOUND_BUSY*)mmap(0, sizeof(unsigned short),
        PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, sndHandle, 0);
      if (pTmp == MAP_FAILED)
      {
        SoundCloseDevices();
//        LogErrorNumber(SOUND_SHARED_MEMORY);
        close(sndHandle);
        // result is already false
      }
      else
      {
        SoundInstance.pSound = pTmp;
        close(sndHandle);
        Result = TRUE;
      }
    }
  }
  return Result;
}

bool SoundOpen()
{
  return TRUE;
}

bool SoundClose()
{
  StopSound();
  return TRUE;
}

bool SoundExit()
{
  // make sure we close before we exit
  SoundClose();
  SoundCloseDevices();
  return TRUE;
}

bool SoundInitialized()
{
  return (SoundInstance.pSound != NULL) &&
         (SoundInstance.pSound != &SoundInstance.Sound);
}

bool SoundTest()
{
  if (SoundInitialized())
    return (*SoundInstance.pSound).Busy == TRUE;
  return FALSE;
}

void SoundReady()
{
  if (!SoundInitialized())
    return;
  while ((*SoundInstance.pSound).Busy)
    usleep(10000);// sleep 10 milliseconds
}

void _soundInitAdPcm()
{
  // Reset ADPCM values to a known and initial value
  SoundInstance.ValPrev = SOUND_ADPCM_INIT_VALPREV;
  SoundInstance.Index   = SOUND_ADPCM_INIT_INDEX;
  SoundInstance.Step    = StepSizeTable[SoundInstance.Index];
}

byte _soundGetAdPcmValue(byte Delta)
{
  short VpDiff;
  byte Sign;

  // Call ONLY when _soundInitAdPcm has been called :-)
  SoundInstance.Step = StepSizeTable[SoundInstance.Index];
  // Find new index value (for later)
  SoundInstance.Index = SoundInstance.Index + IndexTable[Delta];
  if (SoundInstance.Index < 0)
    SoundInstance.Index = 0;
  else
  {
    if (SoundInstance.Index > (STEP_SIZE_TABLE_ENTRIES - 1))
      SoundInstance.Index = STEP_SIZE_TABLE_ENTRIES - 1;
  }
  Sign  = Delta & 8; // Separate sign
  Delta = Delta & 7; // Separate magnitude
  // Compute difference and new predicted value
  VpDiff = SoundInstance.Step >> 3;
  if ((Delta & 4) != 0)
    VpDiff = VpDiff + SoundInstance.Step;
  if ((Delta & 2) != 0)
    VpDiff = VpDiff + (SoundInstance.Step >> 1);
  if ((Delta & 1) != 0)
    VpDiff = VpDiff + (SoundInstance.Step >> 2);

  // "Add" with sign
  if (Sign != 0)
    SoundInstance.ValPrev = SoundInstance.ValPrev - VpDiff;
  else
    SoundInstance.ValPrev = SoundInstance.ValPrev + VpDiff;

  // Clamp value to 8-bit unsigned
  if (SoundInstance.ValPrev > 255)
    SoundInstance.ValPrev = 255;
  else if (SoundInstance.ValPrev < 0)
    SoundInstance.ValPrev = 0;

  // Update step value
  SoundInstance.Step = StepSizeTable[SoundInstance.Index];
  // Return decoded byte (nibble xlated -> 8 bit)
  return (byte)SoundInstance.ValPrev;
}

#define MELODY_FMT 0x0600

void _playRMDFile(char* pFileName, byte volume, bool loop)
{
  SoundInstance.SoundState = SOUND_STATE_IDLE;  // Yes but only shortly
  (*SoundInstance.pSound).Busy = TRUE;
//  SoundInstance.SoundOwner = CallingObjectId();

  sprintf(SoundInstance.PathBuffer, "%s", pFileName);
  // Open SoundFile
  SoundInstance.hSoundFile = open(SoundInstance.PathBuffer, O_RDONLY, 0666);
  if(SoundInstance.hSoundFile >= 0)
  {
    // Get actual FileSize
    stat(SoundInstance.PathBuffer, &SoundInstance.FileStatus);
    SoundInstance.SoundFileLength = SoundInstance.FileStatus.st_size;
    
    if (SoundInstance.SoundFileLength >= 8)
    {
      byte msb, lsb;

      // read the rmd header
      read(SoundInstance.hSoundFile,&msb,1);
      read(SoundInstance.hSoundFile,&lsb,1);
      if (((msb * 256) + lsb) == MELODY_FMT)
      {
        // read data bytes
        read(SoundInstance.hSoundFile,&msb,1);
        read(SoundInstance.hSoundFile,&lsb,1);
        unsigned short dataBytes = (msb * 256) + lsb;
        if ((dataBytes > 0) && ((dataBytes % 4) == 0))
        {
          // tones exist and dataBytes is a multiple of 4
          if (loop)
            SoundInstance.SoundState = SOUND_STATE_FILE_LOOPING;
          else
            SoundInstance.SoundState = SOUND_STATE_FILE;

          // skip 4 bytes
          read(SoundInstance.hSoundFile,&msb,1);
          read(SoundInstance.hSoundFile,&lsb,1);
          read(SoundInstance.hSoundFile,&msb,1);
          read(SoundInstance.hSoundFile,&lsb,1);
          
          // play the tones from the RMD file
          while (dataBytes > 0)
          {
            // read frequency
            read(SoundInstance.hSoundFile,&msb,1);
            read(SoundInstance.hSoundFile,&lsb,1);
            unsigned short freq = (msb * 256) + lsb;
            // read duration
            read(SoundInstance.hSoundFile,&msb,1);
            read(SoundInstance.hSoundFile,&lsb,1);
            unsigned short dur = (msb * 256) + lsb;
            PlayToneEx(freq, dur, volume);
            usleep(dur*1000);
            dataBytes -= 4;
          }
        }
      }
    }
  }
}

void _playSoundSamplesFromStream(int fileHandle)
{
  byte AdPcmData[SOUND_ADPCM_CHUNK];
  unsigned short BytesToRead, BytesRead, BytesWritten, Delta;
  int i;

  // Get new sound data
  // Any new data?
  while (SoundInstance.SoundDataLength > 0)
  {
    SoundInstance.SoundData[0] = SOUND_CMD_SERVICE;
    if (SoundInstance.SoundFileFormat == SOUND_FILE_FORMAT_ADPCM_SOUND)
    {
      // Adjust the chunk size for ADPCM (nibbles) if necessary
      if (SoundInstance.SoundDataLength > SOUND_ADPCM_CHUNK)
        BytesToRead = SOUND_ADPCM_CHUNK;
      else
        BytesToRead = SoundInstance.SoundDataLength;
      // Valid file
      BytesRead = read(fileHandle, AdPcmData, BytesToRead);
      for (i = 0; i < BytesRead; i++)
      {
        SoundInstance.SoundData[2*i + 1] = _soundGetAdPcmValue((AdPcmData[i] >> 4) & 0x0F);
        SoundInstance.SoundData[2*i + 2] = _soundGetAdPcmValue(AdPcmData[i] & 0x0F);
      }
      SoundInstance.BytesToWrite = (byte)(1 + (BytesRead * 2));
    }
    else
    {
      // Non compressed data
      // Adjust the chunk size if necessary
      if (SoundInstance.SoundDataLength > SOUND_CHUNK)
        BytesToRead = SOUND_CHUNK;
      else
        BytesToRead = SoundInstance.SoundDataLength;
      // Valid file
      BytesRead = read(fileHandle, &(SoundInstance.SoundData[1]), BytesToRead);
      SoundInstance.BytesToWrite = BytesRead + 1;
    }

    BytesWritten = 0;
    while (BytesWritten == 0)
    {
      // Now we have or should have some bytes to write down into the driver
      BytesWritten = WriteToSoundDevice(SoundInstance.SoundData, SoundInstance.BytesToWrite); // write bytes
      if (BytesWritten == 0)
        usleep(1000); // delay 1 ms if we were unable to write to the sound device
    }

    // Adjust BytesToWrite with Bytes actually written
    if (BytesWritten > 1)
    {
      Delta = BytesWritten;
      if (SoundInstance.SoundFileFormat == SOUND_FILE_FORMAT_ADPCM_SOUND)
        Delta = Delta / 2;
      SoundInstance.SoundDataLength -= Delta;
      // Buffer data incl. CMD
      SoundInstance.BytesToWrite -= (byte)(BytesWritten + 1);
    }
  }
}

int _readInt(int fileHandle, bool lsb)
{
  byte b1, b2, b3, b4;
  if (lsb)
  {
    read(fileHandle, &b1, 1);
    read(fileHandle, &b2, 1);
    read(fileHandle, &b3, 1);
    read(fileHandle, &b4, 1);
  }
  else
  {
    read(fileHandle, &b4, 1);
    read(fileHandle, &b3, 1);
    read(fileHandle, &b2, 1);
    read(fileHandle, &b1, 1);
  }
  int val = b1 & 0xff;
  val |= (b2 & 0xff) << 8;
  val |= (b3 & 0xff) << 16;
  val |= (b4 & 0xff) << 24;
  return val;
}

short _readShort(int fileHandle, bool lsb)
{
  byte b1, b2;
  if (lsb)
  {
    read(fileHandle, &b1, 1);
    read(fileHandle, &b2, 1);
  }
  else
  {
    read(fileHandle, &b2, 1);
    read(fileHandle, &b1, 1);
  }
  short val = b1 & 0xff;
  val |= (b2 & 0xff) << 8;
  return val;
}

#define RIFF_HDR_SIZE  44
#define RIFF_RIFF_SIG  0x52494646
#define RIFF_WAVE_SIG  0x57415645
#define RIFF_FMT_SIG   0x666d7420
#define RIFF_FMT_PCM   0x0100
#define RIFF_FMT_1CHAN 0x0100
#define RIFF_FMT_8BITS 0x0800
#define RIFF_DATA_SIG  0x64617461

void _playWAVFile(char* pFileName, byte volume, bool loop)
{
  SoundInstance.SoundState = SOUND_STATE_IDLE;  // Yes but only shortly
  (*SoundInstance.pSound).Busy = TRUE;
  sprintf(SoundInstance.PathBuffer, "%s", pFileName);
  // Open SoundFile
  SoundInstance.hSoundFile = open(SoundInstance.PathBuffer, O_RDONLY, 0666);
  if (SoundInstance.hSoundFile >= 0)
  {
    // Get actual FileSize
    stat(SoundInstance.PathBuffer, &SoundInstance.FileStatus);
    SoundInstance.SoundFileLength = SoundInstance.FileStatus.st_size;

    // First check that we have a wave file. File must be at least 44 bytes
    // in size to contain a RIFF header.
    if (SoundInstance.SoundFileLength < RIFF_HDR_SIZE)
      return;

    if (_readInt(SoundInstance.hSoundFile, false) != RIFF_RIFF_SIG)
      return;
    // Skip chunk size
    _readInt(SoundInstance.hSoundFile, true);
    // Check we have a wave file
    if (_readInt(SoundInstance.hSoundFile, false) != RIFF_WAVE_SIG)
      return;
    if (_readInt(SoundInstance.hSoundFile, false) != RIFF_FMT_SIG)
      return;
    // Now check that the format is PCM, Mono 8 bits. Note that these
    // values are stored little endian.
    int sz = _readInt(SoundInstance.hSoundFile, true);
    if (_readShort(SoundInstance.hSoundFile, false) != RIFF_FMT_PCM)
      return;
    if (_readShort(SoundInstance.hSoundFile, false) != RIFF_FMT_1CHAN)
      return;
    int sampleRate = _readInt(SoundInstance.hSoundFile, true);
    _readInt(SoundInstance.hSoundFile, true);
    _readShort(SoundInstance.hSoundFile, true);
    if (_readShort(SoundInstance.hSoundFile, false) != RIFF_FMT_8BITS)
      return;
    // Skip any data in this chunk after the 16 bytes above
    sz -= 16;

    byte Tmp1;
    while (sz-- > 0)
      read(SoundInstance.hSoundFile, &Tmp1, 1);

    int dataLen = 0;
    // Skip optional chunks until we find a data sig (or we hit eof!)
    for(;;)
    {
      int chunk = _readInt(SoundInstance.hSoundFile, false);
      dataLen = _readInt(SoundInstance.hSoundFile, true);
      if (chunk == RIFF_DATA_SIG)
        break;
      // Skip to the start of the next chunk
      while(dataLen-- > 0)
        read(SoundInstance.hSoundFile, &Tmp1, 1);
    }

    // configure settings in SoundInstance record
    SoundInstance.SoundDataLength = dataLen;
    SoundInstance.SoundFileFormat = SOUND_FILE_FORMAT_RAW_SOUND;
    SoundInstance.SoundState = SOUND_STATE_SETUP_FILE;

    byte cmd[2];
    cmd[0] = SOUND_CMD_PLAY;
    cmd[1] = (byte)((volume*8)/100);
    WriteToSoundDevice(cmd, 2); // write 2 bytes

    if (loop)
      SoundInstance.SoundState = SOUND_STATE_FILE_LOOPING;
    else
      SoundInstance.SoundState = SOUND_STATE_FILE;

    _playSoundSamplesFromStream(SoundInstance.hSoundFile);
  }
}

void _playRSOFile(char* pFileName, byte volume, bool loop)
{
  SoundInstance.SoundState = SOUND_STATE_IDLE;  // Yes but only shortly
  (*SoundInstance.pSound).Busy = TRUE;

  sprintf(SoundInstance.PathBuffer, "%s", pFileName);
  // Open SoundFile
  SoundInstance.hSoundFile = open(SoundInstance.PathBuffer, O_RDONLY, 0666);
  if (SoundInstance.hSoundFile >= 0)
  {
    // Get actual FileSize
    stat(SoundInstance.PathBuffer, &SoundInstance.FileStatus);
    SoundInstance.SoundFileLength = SoundInstance.FileStatus.st_size;

    // BIG Endianess

    byte Tmp1, Tmp2;

    read(SoundInstance.hSoundFile,&Tmp1,1);
    read(SoundInstance.hSoundFile,&Tmp2,1);
    SoundInstance.SoundFileFormat = (short)Tmp1 << 8 | (short)Tmp2;

    read(SoundInstance.hSoundFile,&Tmp1,1);
    read(SoundInstance.hSoundFile,&Tmp2,1);
    SoundInstance.SoundDataLength = (short)Tmp1 << 8 | (short)Tmp2;

    read(SoundInstance.hSoundFile,&Tmp1,1);
    read(SoundInstance.hSoundFile,&Tmp2,1);
    SoundInstance.SoundSampleRate = (short)Tmp1 << 8 | (short)Tmp2;

    read(SoundInstance.hSoundFile,&Tmp1,1);
    read(SoundInstance.hSoundFile,&Tmp2,1);
    SoundInstance.SoundPlayMode = (short)Tmp1 << 8 | (short)Tmp2;

    SoundInstance.SoundState = SOUND_STATE_SETUP_FILE;

    if (SoundInstance.SoundFileFormat == SOUND_FILE_FORMAT_ADPCM_SOUND)
      _soundInitAdPcm();

    byte cmd[2];
    cmd[0] = SOUND_CMD_PLAY;
    cmd[1] = (byte)((volume*8)/100);
    WriteToSoundDevice(cmd, 2); // write 2 bytes
    if (loop)
      SoundInstance.SoundState = SOUND_STATE_FILE_LOOPING;
    else
      SoundInstance.SoundState = SOUND_STATE_FILE;

    _playSoundSamplesFromStream(SoundInstance.hSoundFile);
  }
}

void PlayFileEx(char* pFileName, byte volume, bool loop)
{
  if (!SoundInitialized())
    return;

  // sound system must not be muted to play a sound
  if (SoundInstance.SoundMuted != 0)
    return;

  do
  {
    if (strstr(pFileName, ".rmd") != NULL)
    {
      _playRMDFile(pFileName, volume, loop);
    }
    else if (strstr(pFileName, ".wav") != NULL)
    {
      _playWAVFile(pFileName, volume, loop);
    }
    else
    {
      _playRSOFile(pFileName, volume, loop);
    }
  } while (loop && (SoundInstance.StopLoop != 1));
}

void PlayToneEx(unsigned short frequency, unsigned short duration, byte volume)
{
  if (!SoundInitialized())
    return;

  // sound system must not be muted to play a sound
  if (SoundInstance.SoundMuted != 0)
    return;

  (*SoundInstance.pSound).Busy = TRUE;
  byte SoundData[6];
  SoundData[0] = SOUND_CMD_TONE;
  SoundData[1] = (byte)((volume*13)/100);
  SoundData[2] = (byte)(frequency);
  SoundData[3] = (byte)(frequency >> 8);
  SoundData[4] = (byte)(duration);
  SoundData[5] = (byte)(duration >> 8);
  SoundInstance.SoundState = SOUND_STATE_TONE;
  WriteToSoundDevice(SoundData, sizeof(SoundData)); // write 6 bytes
}

void PlaySound(byte aCode)
{
  int i;
  if (!SoundInitialized())
    return;

  // sound system must not be muted to play a sound
  if (SoundInstance.SoundMuted != 0)
    return;

  if (aCode == SOUND_CLICK)
    PlayTone(600, MS_200);
  else if (aCode == SOUND_DOUBLE_BEEP)
  {
    PlayTone(600, MS_150);
    Wait(MS_200);
    PlayTone(600, MS_150);
    Wait(MS_150);
  }
  else if (aCode == SOUND_UP)
    for (i = 4; i < 8; i++)
    {
      PlayTone(TONE_C5 * i / 4, MS_100);
      Wait(MS_100);
    }
  else if (aCode == SOUND_DOWN)
    for (i = 7; i > 3; i--)
    {
      PlayTone(TONE_C5 * i / 4, MS_100);
      Wait(MS_100);
    }
  else if (aCode == SOUND_LOW_BEEP)
  {
    PlayTone(100, MS_500);
    Wait(MS_500);
  }
  else if (aCode == SOUND_FAST_UP)
    for (i = 4; i < 8; i++)
    {
      PlayTone(TONE_C5 * i / 4, MS_50);
      Wait(MS_50);
    }
}

void PlayTonesEx(Tone tones[], size_t size)
{
  int i;
  if (!SoundInitialized())
    return;
    
  // sound system must not be muted to play a sound
  if (SoundInstance.SoundMuted != 0)
    return;
    
  for (i = 0; i <  size; i++) {
    Tone tmp = tones[i];
    PlayTone(tmp.Frequency, tmp.Duration);
    Wait(tmp.Duration);
  }
}

int SoundState()
{
  if (SoundInitialized())
    return SoundInstance.SoundState;
  return SOUND_STATE_IDLE;
}

void StopSound()
{
  if (!SoundInitialized())
    return;
  byte cmd = SOUND_CMD_BREAK;
  SoundInstance.SoundState = SOUND_STATE_IDLE;
  if (SoundInstance.hSoundFile >= 0)
  {
    close(SoundInstance.hSoundFile);
    SoundInstance.hSoundFile  = -1;
  }
  WriteToSoundDevice(&cmd, 1);
}

void MuteSound()
{
  // first stop any playing sounds
  StopSound();
  SoundInstance.SoundMuted = 1;
}

void UnmuteSound()
{
  SoundInstance.SoundMuted = 0;
}

void ClearSound()
{
  // a synonym for StopSound;
  StopSound();
}

