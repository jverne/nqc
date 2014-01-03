/*
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include "USBTowerWin.h"

struct LEGOTowerFunctionName
{
  char* name;
  int offset;
};

#define FUNCTION(name) { "LEGOTower" #name, int(&((LEGOTowerFunctions*)0)->name) },

HINSTANCE LoadLEGOTowerFunctions(LEGOTowerFunctions* list)
{
  static LEGOTowerFunctionName names[] =
    {
    FUNCTION(FindFirst)
    FUNCTION(FindNext)
    FUNCTION(FindPrev)
    FUNCTION(FindStop)
    FUNCTION(Flush)
    FUNCTION(GetCaps)
    FUNCTION(GetCarrierDutyCycle)
    FUNCTION(GetCarrierFrequency)
    FUNCTION(GetCommStats)
    FUNCTION(GetCopyright)
    FUNCTION(GetCredits)
    FUNCTION(GetDefaultConfig)
    FUNCTION(GetDeviceInfo)
    FUNCTION(GetDeviceInfoByNumber)
    FUNCTION(GetEndianness)
    FUNCTION(GetErrorState)
    FUNCTION(GetFactoryConfig)
    FUNCTION(GetHostControllerInfo)
    FUNCTION(GetIRCPacketSize)
    FUNCTION(GetIRCParm)
    FUNCTION(GetIRCTimeFrame)
    FUNCTION(GetIRSpeed)
    FUNCTION(GetLedMode)
    FUNCTION(GetLedState)
    FUNCTION(GetLinkMode)
    FUNCTION(GetPathName)
    FUNCTION(GetPathNameCopy)
    FUNCTION(GetProblemDesc)
    FUNCTION(GetRange)
    FUNCTION(GetState)
    FUNCTION(GetTimeouts)
    FUNCTION(GetVersion)
    FUNCTION(GetWin32Error)
    FUNCTION(IsUSBEnabled)
    FUNCTION(ReenumerateAll)
    FUNCTION(Reset)
    FUNCTION(ResetCommStats)
    FUNCTION(Restart)
    FUNCTION(RestartAll)
    FUNCTION(SetCarrierDutyCycle)
    FUNCTION(SetCarrierFrequency)
    FUNCTION(SetDefaultConfig)
    FUNCTION(SetEndianness)
    FUNCTION(SetIRCPacketSize)
    FUNCTION(SetIRCParm)
    FUNCTION(SetIRCTimeFrame)
    FUNCTION(SetIRSpeed)
    FUNCTION(SetLedMode)
    FUNCTION(SetLedState)
    FUNCTION(SetLinkMode)
    FUNCTION(SetRange)
    FUNCTION(SetTimeouts)
    { 0, 0 }
    };

  HINSTANCE hi = LoadLibrary("towerapi.dll");
  if (!hi) return 0;

  LEGOTowerFunctionName* fn = names;
  while (fn->name)
    {
    FARPROC fp = GetProcAddress(hi, fn->name);
    if (!fp)
      {
      FreeLibrary(hi);
      return 0;
      }
    *(FARPROC*)((char*)list + fn->offset) = fp;
    fn++;
    }

  return hi;
}

LEGOTowerFunctions TOWER;
