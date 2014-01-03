/*
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */
#include <windows.h>
#include "TowerApi.h"

struct LEGOTowerFunctions
{
  BOOL (__stdcall *SetRange)(HANDLE hdevice, WORD wrange);
  BOOL (__stdcall *GetRange)(HANDLE hdevice, WORD *pwrange);
  BOOL (__stdcall *SetLinkMode)(HANDLE hdevice, WORD wlinkmode);
  BOOL (__stdcall *GetLinkMode)(HANDLE hdevice, WORD *pwlinkmode);
  BOOL (__stdcall *SetEndianness)(HANDLE hdevice, WORD wendianness);
  BOOL (__stdcall *GetEndianness)(HANDLE hdevice, WORD *pwendianness);
  BOOL (__stdcall *Flush)(HANDLE hdevice, WORD wflags);
  BOOL (__stdcall *Reset)(HANDLE hdevice);
  BOOL (__stdcall *SetTimeouts)(HANDLE hdevice, DWORD dwread_first, DWORD dwread_ic, DWORD dwwrite);
  BOOL (__stdcall *GetTimeouts)(HANDLE hdevice, DWORD *pdwread_first, DWORD *pdwread_ic, DWORD *pdwwrite);
  BOOL (__stdcall *GetCaps)(HANDLE hdevice, DWORD dwcaps, LT_CAPS *pcaps);
  BOOL (__stdcall *GetVersion)(HANDLE hdevice, PLT_VERSION pver);
  BOOL (__stdcall *GetCopyright)(HANDLE hdevice, LPTSTR lpszbuffer, size_t size);
  BOOL (__stdcall *GetCredits)(HANDLE hdevice, LPTSTR lpszbuffer, size_t size);
  BOOL (__stdcall *GetLedMode)(HANDLE hdevice, BYTE *pwledmode);
  BOOL (__stdcall *SetLedMode)(HANDLE hdevice, BYTE wledmode);
  BOOL (__stdcall *GetLedState)(HANDLE hdevice, BYTE bledid, BYTE *pbcolor);
  BOOL (__stdcall *SetLedState)(HANDLE hdevice, BYTE bledid, BYTE bnewcolor);
  BOOL (__stdcall *GetIRSpeed)(HANDLE hdevice, WORD *pwtxspeed, WORD *pwrxspeed);
  BOOL (__stdcall *SetIRSpeed)(HANDLE hdevice, WORD wtxspeed, WORD wrxspeed);
  BOOL (__stdcall *ResetCommStats)(HANDLE hdevice);
  BOOL (__stdcall *GetCommStats)(HANDLE hdevice, PLT_COMMSTATS pstats);
  BOOL (__stdcall *SetDefaultConfig)(LT_CONFIG *pltcfg);
  BOOL (__stdcall *GetDefaultConfig)(LT_CONFIG *pltcfg);
  BOOL (__stdcall *GetFactoryConfig)(LT_CONFIG *pltcfg);
  BOOL (__stdcall *GetErrorState)(HANDLE hdevice, WORD *pwerror);
  BOOL (__stdcall *GetState)(HANDLE hdevice, WORD *pwtowerstate);
  DWORD (__stdcall *GetWin32Error)(WORD wtowererror);
  BOOL (__stdcall *SetCarrierFrequency)(HANDLE hdevice, WORD wfrequency);
  BOOL (__stdcall *GetCarrierFrequency)(HANDLE hdevice, WORD *pwfrequency);
  BOOL (__stdcall *SetCarrierDutyCycle)(HANDLE hdevice, WORD wrange, WORD wdutycycle);
  BOOL (__stdcall *GetCarrierDutyCycle)(HANDLE hdevice, WORD wrange, WORD *pwdutycycle);
  BOOL (__stdcall *SetIRCParm)(HANDLE hdevice, BYTE bparmcode, BYTE barg, WORD windex);
  BOOL (__stdcall *GetIRCParm)(HANDLE hdevice, BYTE bparmcode, BYTE *pbvalue);
  BOOL (__stdcall *SetIRCTimeFrame)(HANDLE hdevice, BYTE btimeframe);
  BOOL (__stdcall *GetIRCTimeFrame)(HANDLE hdevice, BYTE *pbtimeframe);
  BOOL (__stdcall *SetIRCPacketSize)(HANDLE hdevice, BYTE bpacketsize);
  BOOL (__stdcall *GetIRCPacketSize)(HANDLE hdevice, BYTE *pbpacketsize);
  BOOL (__stdcall *Restart)(HANDLE hdevice);
  BOOL (__stdcall *RestartAll)(void);
  BOOL (__stdcall *ReenumerateAll)(void);
  BOOL (__stdcall *IsUSBEnabled)(void);
  HTOWERINFO (__stdcall *FindFirst)(DWORD dwfindfilter);
  BOOL (__stdcall *FindNext)(HTOWERINFO htower);
  BOOL (__stdcall *FindPrev)(HTOWERINFO htower);
  BOOL (__stdcall *GetDeviceInfo)(HTOWERINFO htower, PLT_TOWERPNPINFO ptowerinfo);
  BOOL (__stdcall *FindStop)(HTOWERINFO htower);
  LPCTSTR (__stdcall *GetPathName)(HTOWERINFO htower);
  BOOL (__stdcall *GetPathNameCopy)(HTOWERINFO htower, LPTSTR pszname, size_t size);
  BOOL (__stdcall *GetProblemDesc)(DWORD dwproblem, LPTSTR pszdesc, size_t size);
  BOOL (__stdcall *GetDeviceInfoByNumber)(DWORD dwdeviceno, DWORD dwfindfilter, PLT_TOWERPNPINFO ppnpinfo);
  BOOL (__stdcall *GetHostControllerInfo)(DWORD dwindex, PLT_HCINFO phcinfo);
};

HINSTANCE LoadLEGOTowerFunctions(LEGOTowerFunctions* list);

extern LEGOTowerFunctions TOWER;
