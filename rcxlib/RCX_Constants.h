/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#ifndef __RCX_Constants_h
#define __RCX_Constants_h

// opcodes

// outputs
#define kRCX_OutputModeOp	0x21
#define kRCX_OutputPowerOp	0x13
#define kRCX_OutputDirOp	0xe1
// inputs
#define kRCX_InputModeOp	0x42
#define kRCX_InputTypeOp	0x32
// sound
#define kRCX_PlaySoundOp	0x51
#define kRCX_PlayToneOp		0x23
// control flow
#define kRCX_TestOp	        0x95
#define kRCX_STestOp            0x85
#define kRCX_JumpOp		0x72
#define kRCX_SJumpOp		0x27
#define kRCX_SetLoopOp		0x82
#define kRCX_CheckLoopOp	0x92
#define kRCX_SCheckLoopOp	0x37
#define kRCX_DecVarJmpLTZeroOp  0xf3
#define kRCX_SDecVarJmpLTZeroOp 0xf2
#define kRCX_ExitEventCheckOp   0xb0
#define kRCX_ExitAccessCtrlOp   0xa0
#define kRCX_SStartEventMonOp   0xb4
#define kRCX_StartEventMonOp    0xb5
#define kRCX_EnterAccessCtrlOp  0x73

// misc
#define kRCX_SetSrcValueOp      0x05
#define kRCX_DelayOp		0x43
#define kRCX_DisplayOp		0x33
#define kRCX_VarOp(code)	(0x14 + ((code) * 16))
#define kRCX_SendMessage	0xb2
#define kRCX_StartTaskOp	0x71
#define kRCX_StopTaskOp		0x81
#define kRCX_StopAllOp		0x50
#define kRCX_ClearTimerOp	0xa1
#define kRCX_ClearMsgOp		0x90
#define kRCX_SendMsgOp		0xb2
#define kRCX_ClearSensorOp	0xd1
#define kRCX_GoSubOp		0x17
#define kRCX_SetDatalogOp	0x52
#define kRCX_DatalogOp		0x62
#define kRCX_UploadDatalogOp	0xa4

// special
#define kRCX_ReadOp		0x12
#define kRCX_UnlockOp		0x15
#define kRCX_BeginTaskOp	0x25
#define kRCX_BeginSubOp		0x35
#define kRCX_DownloadOp		0x45
#define kRCX_DeleteTasksOp	0x40
#define kRCX_DeleteSubsOp	0x70
#define kRCX_BootModeOp		0x65
#define kRCX_BeginFirmwareOp    0x75
#define kRCX_BootOp		0xa5
#define kRCX_PingOp		0x10
#define kRCX_SelectProgramOp	0x91
#define kRCX_BatteryLevelOp	0x30
#define kRCX_SetWatchOp		0x22
#define kRCX_IRModeOp	        0x31
#define kRCX_AutoOffOp	        0xb1

#define kRCX_Message		0xf7
#define kRCX_Remote		0xd2

// cybermaster ops
#define kRCX_DriveOp	        0x41
#define kRCX_OnWaitOp	        0xc2
#define kRCX_OnWaitDifferentOp	0x53
#define kRCX_ClearTachoOp	0x11

// some scout ops
#define kRCX_GOutputModeOp	0x67
#define kRCX_GOutputPowerOp	0xa3
#define kRCX_GOutputDirOp	0x77
#define kRCX_PollMemoryOp	0x63

// some spybot ops
#define kRCX_PushStackEntryOp   0xe3
#define kRCX_PopStackEntryOp    0x01
#define kRCX_UploadEepromOp     0x11
#define kRCX_FindOp             0xd5
#define kRCX_ClearRelTableOp    0x36
#define kRCX_PlaySysMoodOp      0xc7
#define kRCX_PlaySysSndVarOp    0xe7

typedef enum
{
	kRCX_VariableType	 = 0,
	kRCX_TimerType,
	kRCX_ConstantType,
	kRCX_OutputStatusType,
	kRCX_RandomType,
	kRCX_TachCounterType,
	kRCX_TachSpeedType,
	kRCX_OutputCurrentType,
	kRCX_ProgramType,
	kRCX_InputValueType,
	kRCX_InputTypeType,
	kRCX_InputModeType,
	kRCX_InputRawType,
	kRCX_InputBooleanType,
	kRCX_WatchType,
	kRCX_MessageType,
	kRCX_AGCType, // 16
        kRCX_GlobalMotorStatusType, // = 17;
        kRCX_SpybotStackType, //       = 18; // spybot only
        kRCX_SpybotTimerCtrlType, //   = 19; // spybot only
        kRCX_SpybotEepromType, //      = 20; // spybot only
        kRCX_CounterType, //           = 21; // RCX2, scout, & spybot only
        kRCX_SpybotLEDType, //         = 22; // spybot only
        kRCX_TaskEventsType, //        = 23; // rcx2, scout, spybot
        kRCX_ScoutEventFBType, //      = 24; // scout only
        kRCX_EventStateType, //        = 25; // rcx2 & spybot
        kRCX_TenMSTimerType, //        = 26; // rcx2 & spybot fast timer
        kRCX_ClickCounterType, //      = 27; // rcx2
        kRCX_UpperThresholdType, //    = 28; // rcx2 & spybot
        kRCX_LowerThresholdType, //    = 29; // rcx2 & spybot
        kRCX_HysteresisType, //        = 30; // rcx2 & spybot
        kRCX_DurationType, //          = 31; // rcx2 & spybot
        kRCX_SpybotTaskIDType, //      = 32; // spybot only
        kRCX_UARTSetupType, //         = 33; // rcx2 & spybot
        kRCX_BatteryLevelType, //      = 34; // rcx2 & spybot
        kRCX_FirmwareVersionType, //   = 35; // rcx2 & spybot
        kRCX_IndirectType, //          = 36
        kRCX_DatalogTypeInd, //        = 37
        kRCX_DatalogType, //           = 38
        kRCX_DatalogValueInd, //       = 39
        kRCX_DatalogValue, //          = 40
        kRCX_DatalogByteInd, //        = 41
        kRCX_DatalogByte, //           = 42
        kRCX_SpybotGameNotesType, //   = 43; // spybot only
        kRCX_SpybotType44, //          = 44; // ????????
        kRCX_SpybotRobotDistType, //   = 45; // spybot only
        kRCX_SpybotRobotDirType, //    = 46; // spybot only
        kRCX_SpybotRobotOrientType, // = 47; // spybot only
        kRCX_SpybotType48, //          = 48; // ????????
        kRCX_SpybotRobotIDType, //     = 49; // spybot only
        kRCX_SpybotRobotTargetType, // = 50; // spybot only
        kRCX_SpybotPingCtrlType, //    = 51; // spybot only
        kRCX_SpybotBeaconCtrlType, //  = 52; // spybot only
        kRCX_SpybotSoundCtrlType, //   = 53; // spybot only
        kRCX_SpybotIndEepromType //    = 54; // spybot only
} RCX_ValueType;

const RCX_ValueType kRCX_NoType = (RCX_ValueType) -1;

// Swan types
const RCX_ValueType kRCX_GlobalVariableType = (RCX_ValueType) 43;
const RCX_ValueType kRCX_IndirectGlobalVarType = (RCX_ValueType) 44;
const RCX_ValueType kRCX_IndexedArrayType = (RCX_ValueType) 6;

typedef int RCX_Value;

#define kRCX_ValueUsesTemp	0x1000000	// flag used for indirects
#define RCX_VALUE(t, d)		(((int)(t) << 16) | ((d) & 0xffff))
#define RCX_VALUE_TYPE(v)	((RCX_ValueType)(((v)>>16) & 0xff))
#define RCX_VALUE_DATA_INT(v)	((v) & 0xffff)
#define RCX_VALUE_DATA(v)	((short)RCX_VALUE_DATA_INT(v))

typedef enum
{
	kRCX_SetVar = 0,
	kRCX_AddVar,
	kRCX_SubVar,
	kRCX_DivVar,
	kRCX_MulVar,
	kRCX_SgnVar,
	kRCX_AbsVar,
	kRCX_AndVar,
	kRCX_OrVar,
        kRCX_NotVar, // Swan only
        kRCX_ModVar, // Swan only
        kRCX_ShlVar, // Swan only
        kRCX_ShrVar, // Swan only
        kRCX_XOrVar, // Swan only
	kRCX_IllegalVar = -1
} RCX_VarCode;


typedef enum
{
	kRCX_OutputFloat = 0,
	kRCX_OutputOff = 0x40,
	kRCX_OutputOn = 0x80
} RCX_OutputMode;


typedef enum
{
	kRCX_OutputBackward = 0,
	kRCX_OutputToggle = 0x40,
	kRCX_OutputForward = 0x80
} RCX_OutputDir;


typedef enum
{
	kRCX_InputRaw = 0,
	kRCX_InputBoolean = 0x20,
	kRCX_InputEdgeCounter = 0x40,
	kRCX_InputPeriodicCounter = 0x60,
	kRCX_InputPercentage = 0x80,
	kRCX_InputCelcius = 0xa0,
	kRCX_InputFahrenheit = 0xc0,
	kRCX_InputAngle = 0xe0
} RCX_InputMode;

typedef enum
{
	kRCX_InputNone = 0,
	kRCX_InputSwitch,
	kRCX_InputTemp,
	kRCX_InputLight,
	kRCX_InputRotation,
	kRCX_InputID0,
	kRCX_InputID1,
	kRCX_InputID2
} RCX_InputType;


typedef enum
{
	kRCX_LessOrEqual = 0,
	kRCX_GreaterOrEqual,
	kRCX_NotEqualTo,
	kRCX_EqualTo
} RCX_Relation;


typedef enum
{
	kRCX_TaskChunk = 0,
	kRCX_SubChunk,
	kRCX_SoundChunk,
        kRCX_AnimationChunk,
//        kRCX_DataChunk,

	kRCX_ChunkTypeCount // must be last entry
} RCX_ChunkType;


// Swan support code
//typedef char byte;
typedef unsigned char ubyte;

typedef enum
{
  arithOffsetAddTo          = 0, // Must be consecutive opcodes
  arithOffsetMinusTo        = 1,
  arithOffsetTimesTo        = 2,
  arithOffsetDivideTo       = 3,
  arithOffsetAndTo          = 4,
  arithOffsetOrTo           = 5,
  arithOffsetBitComplement  = 6,
  arithOffsetModuloTo       = 7  // Don't change order. Used as bounds in 'for' statements
} TArithOffsets;

typedef enum
{
  rcxNoop                         =   0x00, // Uncertain of this. But keep it in
  rcxDebugGetStatus               =   0x01, // New
  rcxPlayToneVar                  =   0x02,
  rcxDirectEvent                  =   0x03,
  rcxCalibrateEvent               =   0x04,
  rcxSetSourceValue               =   0x05,
  rcxClearAllEvents               =   0x06,
  rcxDebugUnsolicitedReport       =   0x07, // New - actually not an opcode but a message generated by firmware. Needs an opcode value

  // new arithmetic opcodes
  rcxAddToSourceValue             =  (ubyte)  0x08, // Must be consecutive opcodes
  rcxMinusToSourceValue           =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetMinusTo),
  rcxTimesToSourceValue           =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetTimesTo),
  rcxDivideToSourceValue          =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetDivideTo),
  rcxAndToSourceValue             =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetAndTo),
  rcxOrToSourceValue              =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetOrTo),
  rcxBitComplementSourceValue     =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetBitComplement),
  rcxModuloToSourceValue          =  (ubyte) (rcxAddToSourceValue + (ubyte) arithOffsetModuloTo),  //0xDF

  rcxAlive                        =   0x10,
  rcxSetSourceValueByteConst      =   0x11, // New
  rcxGetValue                     =   0x12,
  rcxSetMotorPower                =   0x13,
  rcxAssign                       =   0x14,
  rcxGetVersion                   =   0x15,
  rcxDebugClearException          =   0x16, // New
  rcxCallSub                      =   0x17,

  // New arithmetic opcodes
  rcxAddToLong                    =  (ubyte)  0x18, // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToLong                  =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetMinusTo),
  rcxTimesToLong                  =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetTimesTo),
  rcxDivideToLong                 =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetDivideTo),
  rcxAndToLong                    =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetAndTo),
  rcxOrToLong                     =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetOrTo),
  rcxBitComplementLong            =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetBitComplement),
  rcxModuloToLong                 =  (ubyte) (rcxAddToLong + (ubyte) arithOffsetModuloTo),  //0xDF

  rcxGetMemoryMap                 =   0x20,
  rcxSetMotorOnOffFloat           =   0x21,
  rcxSetTime                      =   0x22,
  rcxPlayTone                     =   0x23,
  rcxAddTo                        =   0x24,
  rcxStartTaskDownload            =   0x25,
  rcxAssignLong                   =   0x26, // New
  rcxBranchNear                   =   0x27,

  rcxAssignLongConstantWord       =   0x28, // Must be consecutive opcodes and op % 8 == 0
  rcxAssignLongConstant           =   0x29, // New
  rcxArrayBounds                  =   0x2A, // New
  rcxAssignGlobalConstantByte     =   0x2B, // New - cannot be used for message opcode. Conflict, and length wrong
  rcxAssignGlobalConstant         =   0x2C, // New - cannot be used for message opcode. Conflict, and length wrong
  rcxAssignGlobalVariable         =   0x2D, // New - cannot be used for message opcode. Conflict, and length wrong
  rcxBitComplement                =   0x2E, // New opcode
  rcxAssignSourceValue            =   0x2F, // New

  rcxGetBatteryVoltage            =   0x30,
  rcxSetXmitRange                 =   0x31,
  rcxSetSensorType                =   0x32,
  rcxSetDisplay                   =   0x33,
  rcxMinusTo                      =   0x34,
  rcxStartSubroutineDownload      =   0x35,
  rcxSetSourceValueShortVariable  =   0x36, // New
  rcxPopLoopCounterShort          =   0x37,

  // New arithmetic opcodes
  rcxAddToFloat                   =  (ubyte)  0x38, // Must be consecutive opcodes
  rcxMinusToFloat                 =  (ubyte) (rcxAddToFloat + (ubyte) arithOffsetMinusTo),
  rcxTimesToFloat                 =  (ubyte) (rcxAddToFloat + (ubyte) arithOffsetTimesTo),
  rcxDivideToFloat                =  (ubyte) (rcxAddToFloat + (ubyte) arithOffsetDivideTo),

  /*
  //rcxAddToFloatConstant           =  (ubyte)  0x3C, // Must be consecutive opcodes
  //rcxMinusToFloatConstant         =  (ubyte) (rcxAddToFloatConstant + (ubyte) arithOffsetMinusTo),
  //rcxTimesToFloatConstant         =  (ubyte) (rcxAddToFloatConstant + (ubyte) arithOffsetTimesTo),
  //rcxDivideToFloatConstant        =  (ubyte) (rcxAddToFloatConstant + (ubyte) arithOffsetDivideTo),
  */
  rcxTestAndBranchLongBigNear     =   0x3C, // New
  rcxTestAndBranchLongBigFar      =   0x3D, // New
  rcxTestAndBranchIntBigFar       =   0x3E, // New
  rcxTestAndBranchFloatBigFar     =   0x3F, // New

  rcxDeleteAllTasks               =   0x40,
  rcxSendMessageWith1ParmByte     =   0x41, // New
  rcxSetSensorMode                =   0x42,
  rcxWaitTimer10MSec              =   0x43,
  rcxDivideTo                     =   0x44,
  rcxTransferData                 =   0x45, // Note this is same as reply to rcxSendMessage(0xB2/0xB8)
  rcxUploadEventMap               =   0x46, // New
  rcxWaitTimer1MSec               =   0x47, // New

  // new arithmetic opcodes
  rcxAddToGlobalUbyte             =  (ubyte)  0x48,  // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToGlobalUbyte           =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetMinusTo),
  rcxTimesToGlobalUbyte           =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetTimesTo),
  rcxDivideToGlobalUbyte          =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetDivideTo),
  rcxAndToGlobalUbyte             =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetAndTo),
  rcxOrToGlobalUbyte              =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetOrTo),
  rcxBitComplementGlobalByte      =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetBitComplement),
  rcxModuloToGlobalUbyte          =  (ubyte) (rcxAddToGlobalUbyte + (ubyte) arithOffsetModuloTo),  //0x4F

  rcxStopAllTasks                 =   0x50,
  rcxPlaySound                    =   0x51,
  rcxSetDatalogSize               =   0x52,
  rcxBranchNearByte               =   0x53, // New
  rcxTimesTo                      =   0x54,
  rcxBranchFarWord                =   0x55, // New
  // don't use 0x55 as an opcode that can be sent via message or it will screw up recognition of message headers
  rcxTestAndBranchIntConstShortNear    =  (ubyte)  0x56, // New
  rcxTestAndBranchIntConstBigNear      =  (ubyte)  0x57, // New

  // new arithmetic opcodes
  rcxAddToGlobalConstant          =  (ubyte)  0x58,  // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToGlobalConstant        =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetMinusTo),
  rcxTimesToGlobalConstant        =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetTimesTo),
  rcxDivideToGlobalConstant       =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetDivideTo),
  rcxAndToGlobalConstant          =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetAndTo),
  rcxOrToGlobalConstant           =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetOrTo),
  rcxBitComplementGlobalConstant  =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetBitComplement),
  rcxModuloToGlobalConstant       =  (ubyte) (rcxAddToGlobalConstant + (ubyte) arithOffsetModuloTo),  //0x5F

  rcxPowerOff                     =   0x60,
  rcxDeleteTask                   =   0x61,
  rcxDatalogNext                  =   0x62,
  rcxGetProgramDataBytes          =   0x63,
  rcxSignVariable                 =   0x64,
  rcxDeleteFirmware               =   0x65,
  rcxAssert                       =   0x66, // New
  rcxSetGlobalMotorControl        =   0x67,

  // new arithmetic opcodes
  rcxAddToLongConstant            =  (ubyte)  0x68,  // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToLongConstant          =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetMinusTo),
  rcxTimesToLongConstant          =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetTimesTo),
  rcxDivideToLongConstant         =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetDivideTo),
  rcxAndToLongConstant            =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetAndTo),
  rcxOrToLongConstant             =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetOrTo),
  rcxBitComplementLongConstant    =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetBitComplement),
  rcxModuloToLongConstant         =  (ubyte) (rcxAddToLongConstant + (ubyte) arithOffsetModuloTo),

  rcxDeleteAllSubs                =   0x70,
  rcxStartTask                    =   0x71,
  rcxBranchFar                    =   0x72,
  rcxEnterAccessControl           =   0x73,
  rcxAbsoluteValue                =   0x74,
  rcxStartFirmwareDownload        =   0x75,
  rcxAssignLongConstantByte       =   0x76, // New
  rcxSetNormSetInvAltDir          =   0x77,

  rcxTestAndBranchIntShortConstNear =  (ubyte)  0x78, // New
  rcxTestAndBranchIntBigConstNear   =  (ubyte)  0x79, // New
  rcxWaitTimerValue                 =  (ubyte)  0x7A, // New
  rcxAbsoluteValueSourceValue       =  (ubyte)  0x7B, // New
  rcxSignValueSourceValue           =  (ubyte)  0x7C, // New

  rcxBitTest                      =  (ubyte)  0x7D, // New
  rcxBitSet                       =  (ubyte)  0x7E, // New
  rcxBitClear                     =  (ubyte)  0x7F, // New

  rcxClearSound                   =  (ubyte)  0x80,
  rcxStopTask                     =  (ubyte)  0x81,
  rcxSetLoopCounter               =  (ubyte)  0x82,
  rcxUploadGlobalVariables        =  (ubyte)  0x83, // New opcode
  rcxAndTo                        =  (ubyte)  0x84,
  rcxTestAndBranchNear            =  (ubyte)  0x85,
  rcxTestAndBranchFloatShortFar   =  (ubyte)  0x86, // New
  rcxUploadEvent                  =  (ubyte)  0x87, // New opcode

  rcxXOr                          =  (ubyte)  0x88, // New
  rcxTestAndBranchFloatBigNear    =  (ubyte)  0x89, // New
  rcxTestAndBranchFloatShortNear  =  (ubyte)  0x8A, // New
  rcxTestAndBranchLongShortNear   =  (ubyte)  0x8B, // New
  rcxFloatToLong                  =  (ubyte)  0x8C, // New
  rcxIntegerToLong                =  (ubyte)  0x8D, // New
  rcxTestAndBranchIntShortNear    =  (ubyte)  0x8E, // New
  rcxTestAndBranchIntBigNear      =  (ubyte)  0x8F, // New

  rcxClearMessage                 =  (ubyte)  0x90,
  rcxSetProgramNumber             =  (ubyte)  0x91,
  rcxPopLoopCounter               =  (ubyte)  0x92,
  rcxSetEventDefinition           =  (ubyte)  0x93,
  rcxOrTo                         =  (ubyte)  0x94,
  rcxTestAndBranchFar             =  (ubyte)  0x95,
  rcxTestAndBranchLongShortFar    =  (ubyte)  0x96, // New
  rcxIncCounter                   =  (ubyte)  0x97,

  rcxShiftLeft                    =  (ubyte)  0x98, // New
  rcxShiftRight                   =  (ubyte)  0x99, // New
  rcxTrinaryTimesConstant         =  (ubyte)  0x9A, // New
  rcxNegate                       =  (ubyte)  0x9B, // New
  // unused 0x9C
  // unused 0x9D
  rcxSwitchIndexTableNear         =  (ubyte)  0x9E, // New
  rcxSwitchIndexTableFar          =  (ubyte)  0x9F, // New

  rcxExitAccessControl            =  (ubyte)  0xA0,
  rcxClearTimer                   =  (ubyte)  0xA1,
  rcxSetMessageByteParm           =  (ubyte)  0xA2, // New opcode -- message ID plus one byte parm
  rcxSetMaxPower                  =  (ubyte)  0xA3,
  rcxUploadDatalog                =  (ubyte)  0xA4,
  rcxUnlockFirmware               =  (ubyte)  0xA5,
  rcxTrinaryMinusConstant         =  (ubyte)  0xA6, // New
  rcxDecCounter                   =  (ubyte)  0xA7,

  rcxTrinaryAdd                   =  (ubyte)  0xA8, // New
  rcxTrinaryMinus                 =  (ubyte)  0xA9, // New
  rcxTrinaryTimes                 =  (ubyte)  0xAA, // New
// Don't use 0xAA [~0x55] as an opcode that can be sent via spurious message or reply could screw up recognition of message headers
  rcxTrinaryAddConstant           =  (ubyte)  0xAB, // New
  rcxIntegerToFloat               =  (ubyte)  0xAC, // New opcode
  rcxLongToFloat                  =  (ubyte)  0xAD, // New opcode
  rcxFloatToInteger               =  (ubyte)  0xAE, // New opcode
  rcxLongToInteger                =  (ubyte)  0xAF, // New opcode

  rcxExitEventCheck               =  (ubyte)  0xB0,
  rcxSetPowerDownDelay            =  (ubyte)  0xB1,
  rcxSendMessage                  =  (ubyte)  0xB2, // Note this is same as reply to rcxTransferData(0x45)
  rcxSetMessageWordParm           =  (ubyte)  0xB3, // New opcode -- message ID plus one byte parm
  rcxStartEventMonitorShort       =  (ubyte)  0xB4,
  rcxStartEventMonitorLong        =  (ubyte)  0xB5,
  rcxUploadTimers                 =  (ubyte)  0xB6, // New opcode
  rcxClearCounter                 =  (ubyte)  0xB7,

  // new arithmetic opcodes
  rcxAddToShortVar                =  (ubyte)  0xB8, // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToShortVar              =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetMinusTo),
  rcxTimesToShortVar              =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetTimesTo),
  rcxDivideToShortVar             =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetDivideTo),
  rcxAndToShortVar                =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetAndTo),
  rcxOrToShortVar                 =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetOrTo),
  rcxBitComplementShortVar        =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetBitComplement),
  rcxModuloToShortVar             =  (ubyte) (rcxAddToShortVar + (ubyte) arithOffsetModuloTo), //0xBF

  // unused 0xC0
  rcxDeleteSub                    =  (ubyte)  0xC1,
  rcxSendUARTData                 =  (ubyte)  0xC2,
  rcxSendMessageWithParm          =  (ubyte)  0xC3, // New opcode -- allows message plus optional parm
  rcxSetMessageVariableParm       =  (ubyte)  0xC4, // New
  rcxSwitch                       =  (ubyte)  0xC5, // New
  rcxSwitchByteCase               =  (ubyte)  0xC6, // New
  rcxClearAll                     =  (ubyte)  0xC7, // New opcode

  // new arithmetic opcodes
  rcxAddToGlobal                  =  (ubyte)  0xC8, // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToGlobal                =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetMinusTo),
  rcxTimesToGlobal                =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetTimesTo),
  rcxDivideToGlobal               =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetDivideTo),
  rcxAndToGlobal                  =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetAndTo),
  rcxOrToGlobal                   =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetOrTo),
  rcxBitComplementGlobal          =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetBitComplement),
  rcxModuloToGlobal               =  (ubyte) (rcxAddToGlobal + (ubyte) arithOffsetModuloTo),  //0xCF

  rcxMuteSound                    =  (ubyte)  0xD0,
  rcxClearSensorValue             =  (ubyte)  0xD1,
  rcxRemoteControlCommand         =  (ubyte)  0xD2,
  rcxPlayKHertz                   =  (ubyte)  0xD3, // New
  rcxGetValueLong                 =  (ubyte)  0xD4, // New -- not implemented
  rcxTestAndBranchIntShortFar     =  (ubyte)  0xD5, // New
  rcxUploadDeviceVariables        =  (ubyte)  0xD6, // New opcode
  rcxSetPriority                  =  (ubyte)  0xD7,

  // new arithmetic opcodes
  rcxAddToShortVarConstant        =  (ubyte)  0xD8, // Must be consecutive opcodes and op % 8 == 0
  rcxMinusToShortVarConstant      =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetMinusTo),
  rcxTimesToShortVarConstant      =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetTimesTo),
  rcxDivideToShortVarConstant     =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetDivideTo),
  rcxAndToShortVarConstant        =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetAndTo),
  rcxOrToShortVarConstant         =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetOrTo),
  rcxBitComplementShortVarConstant=  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetBitComplement),
  rcxModuloToShortVarConstant     =  (ubyte) (rcxAddToShortVarConstant + (ubyte) arithOffsetModuloTo), //0xDF

  rcxUnmuteSound                  =  (ubyte)  0xE0,
  rcxSetMotorDirection            =  (ubyte)  0xE1,
  rcxDebugSuspendResumeStep       =  (ubyte)  0xE2, // New opcode
#ifndef KHxToneSupported
  rcxPlayKHertzVariable           =  (ubyte)  0xE3, // New
#endif
  rcxDebugSetBreakPoint           =  (ubyte)  0xE4, // New -- .
  rcxSetDisplayVariable           =  (ubyte)  0xE5, // store a variable to update in the display

  // New breakpoint opcodes
  rcxBreakpoint0                  =  (ubyte)  0xE6, // Must be consecutive opcodes
  rcxBreakpoint1                  =  (ubyte)  0xE7,
  rcxBreakpoint2                  =  (ubyte)  0xE8,
  rcxBreakpoint3                  =  (ubyte)  0xE9,
  rcxBreakpoint4                  =  (ubyte)  0xEA,
  rcxBreakpoint5                  =  (ubyte)  0xEB,
  rcxBreakpoint6                  =  (ubyte)  0xEC,
  rcxBreakpoint7                  =  (ubyte)  0xED,
  rcxBreakpoint8                  =  (ubyte)  0xEE,
  rcxBreakpoint9                  =  (ubyte)  0xEF,

  // unused 0xF0
  rcxSetMotorState                =  (ubyte)  0xF1, // New
  rcxDecVarJumpLTZeroShort        =  (ubyte)  0xF2,
  rcxDecVarJumpLTZeroLong         =  (ubyte)  0xF3,
  rcxDebugSetProgramCounter       =  (ubyte)  0xF4, // New -- .
  rcxModuloTo                     =  (ubyte)  0xF5, // New -- .
  rcxReturn                       =  (ubyte)  0xF6,
  rcxSetMessage                   =  (ubyte)  0xF7,

  // new arithmetic opcodes
  rcxAddToGenericParms            =  (ubyte)  0xF8, // Must be consecutive opcodes and op % 8     =    = 0
  rcxMinusToGenericParms          =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetMinusTo),
  rcxTimesToGenericParms          =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetTimesTo),
  rcxDivideToGenericParms         =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetDivideTo),
  rcxAndToGenericParms            =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetAndTo),
  rcxOrToGenericParms             =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetOrTo),
  rcxBitComplementGenericParms    =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetBitComplement),
  rcxModuloToGenericParms         =  (ubyte) (rcxAddToGenericParms + (ubyte) arithOffsetModuloTo), //0xFF

  rcxFirst                        =  (ubyte)  0x00,
  rcxBad                          =  (ubyte)  0xFF, // May be something else
  rcxLast                         =  (ubyte)  0xFF
} TRCXop;


#endif
