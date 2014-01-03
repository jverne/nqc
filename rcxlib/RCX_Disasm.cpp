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

#include <cstdio>
#include <cstring>
#include <stack>
#include <vector>

#include "RCX_Disasm.h"
#include "RCX_Constants.h"
#include "RCX_SourceTag.h"

using std::sprintf;
using std::stack;
using std::vector;

#ifdef DEBUG
using std::printf;
//#define CHECK_LENGTHS
#endif

static int ArgsLength(ULong args);
static void SPrintOutputNames(char *argText, const UByte outs);
static int ComputeOffset(UByte b1, UByte b2=0, bool lowFirst = true);

#define LOOKUP(i,a)	(((unsigned)(i)<sizeof(a)/sizeof(char*)) ? a[i] : "?")
#define WORD(ptr)	((short)((((ptr)[1]) << 8) + ((ptr)[0])))
#define HLWORD(ptr)	((short)((((ptr)[0]) << 8) + ((ptr)[1])))

static const char *inputTypeNames[] = { "None", "Switch", "Temp", "Light", "Angle" };
static const char *inputModeNames[] = { "Raw", "Boolean", "Edge", "Pulse", "Percent", "Celcius", "Fahrenheit", "Angle" };
static const char *outputDirName[] = { "Rev", "Flip", "Fwd" };
static const char *outputModeName[] = { "Float", "Off", "On" };
static const char *relNames[] = { "<=", ">=", "!=", "==" };
static const char *findRelNames[] = { ">", "<", "=", "!=" };

static const char *typeNames[] = {
    "Var", "Timer", "Const", "Output", "Random",
    "%5", "%6", "%7", "Program", "Input",
    "SensorType", "SensorMode", "SensorRaw", "SensorBool",
    "Watch", "Message", "%16", "GlobalOutput", "%18",
    "%19", "%20", "Counter", "%22", "ActiveEvents",
    "%24", "EventState", "FastTimer", "ClickCounter",
    "UpperLimit", "LowerLimit", "Hysteresis", "ClickTime",
    "%32", "UART", "BatteryLevel", "FirmwareVersion",
    "Indirect", "LogTypeInd", "LogTypeDir",
    "LogValInd", "LogValDir", "LogByteInd",
    "LogByteDir"
};

static const char *swanTypeNames[] = {
    "Var", "Timer", "Const", "Output", "Random",
    "PwrSigned", "IntIndG", "BrakePwr", "Program", "Input",
    "SensorType", "SensorMode", "SensorRaw", "SensorBool",
    "Watch", "Message", "Power128", "GlobalOutput", "EventType",
    "Event", "EventCnt", "Counter", "MSTimer", "ActiveEvents",
    "System", "EventState", "FastTimer", "ClickCounter",
    "UpperLimit", "LowerLimit", "Hysteresis", "ClickTime",
    "Power8", "UART", "BatteryLevel", "FirmwareVersion",
    "Indirect", "LogTypeInd", "LogTypeDir", "LogValInd", "LogValDir",
    "LogByteInd", "LogByteDir", "GlobalVar", "IndGlobalInt", "IndGlobalLong",
    "IndGlobalFloat", "IdxGlobalConst", "IdxGlobalLongConst",
    "StackVar", "ConstantVar",
    "RetValWord", "RetValLong", "RetValFloat",
    "VarByte", "VarWord", "VarLong",
    "TaskStackVarByte", "TaskStackVarWord", "TaskStackVarLong",
    "TaskVar", "TaskStackAddress", "TaskStackSize", "Bad"
};

enum DataFormat
{
	kAF_None = 0,
	kAF_Skip8,
	kAF_Skip16,
	kAF_Raw8,
	kAF_Raw16,
	kAF_Value8,
	kAF_Value16,
	kAF_Var,
        kAF_SrcRelThresh,

	kAF_Outputs,
	kAF_OutputMode,
	kAF_OutputDir,
	kAF_InputType,
	kAF_InputMode,

	kAF_Condition,
	kAF_Jump8,
	kAF_Jump16,
	kAF_Offset8,
	kAF_Offset16,

        kAF_GVar,
        kAF_HLRaw16,
        kAF_CondV2V2,
        kAF_HLOffset16,
        kAF_CondV1V1,
        kAF_CondC1V1,
        kAF_CondV1C1,
        kAF_CondV2C2,
        kAF_CondC2V2
};

static const int argFormatLengths[]={
	0, 1, 2, 1, 2, 2, 3, 1, 4,
	1, 1, 1, 1, 1,
	5, 1, 2, 1, 2,
        1, 2, 6, 2, 4, 4, 4, 6, 6
};


#define kArgFormatWidth 5
#define kArgFormatMask	((1 << kArgFormatWidth) - 1)

#define ARG(index, format)	((format) << (index) * kArgFormatWidth)

#define ARGS1(f0)				(ARG(0,f0))
#define ARGS2(f0,f1)			(ARG(0,f0) + ARG(1,f1))
#define ARGS3(f0,f1,f2)			(ARG(0,f0) + ARG(1,f1) + ARG(2,f2))
#define ARGS4(f0,f1,f2,f3)		(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3))
#define ARGS5(f0,f1,f2,f3,f4)		(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3) + ARG(4,f4))
#define ARGS6(f0,f1,f2,f3,f4,f5)	(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3) + ARG(4,f4) + ARG(5,f5))
#define ARGS7(f0,f1,f2,f3,f4,f5,f6)	(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3) + ARG(4,f4) + ARG(5,f5) + ARG(6,f6))
#define ARGS8(f0,f1,f2,f3,f4,f5,f6,f7)	(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3) + ARG(4,f4) + ARG(5,f5) + ARG(6,f6) + ARG(7,f7))
#define ARGS9(f0,f1,f2,f3,f4,f5,f6,f7,f8)	(ARG(0,f0) + ARG(1,f1) + ARG(2,f2) + ARG(3,f3) + ARG(4,f4) + ARG(5,f5) + ARG(6,f6) + ARG(7,f7) + ARG(8,f8))

// this list should contain all instructions that are multi-platform (generally RCX2)
static RCX_Disasm::Instruction sGenericInstructions[] = {

{ "out", 0x21, ARGS1(kAF_OutputMode) },
{ "pwr", 0x13, ARGS2(kAF_Outputs, kAF_Value8) },
{ "dir", 0xe1, ARGS1(kAF_OutputDir) },
{ "senm", 0x42, ARGS2(kAF_Raw8, kAF_InputMode) },
{ "sent", 0x32, ARGS2(kAF_Raw8, kAF_InputType) },

{ "wait", 0x43, ARGS1(kAF_Value16) },
{ "playt", 0x23, ARGS2(kAF_Raw16, kAF_Raw8) },
{ "plays", 0x51, ARGS1(kAF_Raw8) },
{ "view", 0x33, ARGS1(kAF_Value16) },

{ "setv", 0x14, ARGS2(kAF_Var, kAF_Value16) },
{ "sumv", 0x24, ARGS2(kAF_Var, kAF_Value16) },
{ "subv", 0x34, ARGS2(kAF_Var, kAF_Value16) },
{ "divv", 0x44, ARGS2(kAF_Var, kAF_Value16) },
{ "mulv", 0x54, ARGS2(kAF_Var, kAF_Value16) },
{ "andv", 0x84, ARGS2(kAF_Var, kAF_Value16) },
{ "orv", 0x94, ARGS2(kAF_Var, kAF_Value16) },
{ "absv", 0x74, ARGS2(kAF_Var, kAF_Value16) },
{ "sgnv", 0x64, ARGS2(kAF_Var, kAF_Value16) },

{ "start", 0x71, ARGS1(kAF_Raw8) },
{ "stop", 0x81, ARGS1(kAF_Raw8) },
{ "stop", 0x50, 0},

{ "msgz", 0x90, 0},
{ "msg", 0xb2, ARGS1(kAF_Value8) },
{ "senz", 0xd1, ARGS1(kAF_Raw8) },
{ "logz", 0x52, ARGS1(kAF_Raw16)},
{ "log", 0x62, ARGS1(kAF_Value8) },
{ "polld", 0xa4, ARGS2(kAF_Raw16, kAF_Raw16)},
{ "setw", 0x22, ARGS2(kAF_Raw8, kAF_Raw8) },
{ "txs", 0x31, ARGS1(kAF_Raw8) },
{ "offp", 0x60, 0},

{ "tmrz", 0xa1, ARGS1(kAF_Raw8) },
{ "calls", 0x17, ARGS1(kAF_Raw8) },

{ "loops", 0x82, ARGS1(kAF_Value8)},
{ "loopc", 0x37, ARGS1(kAF_Offset8) },
{ "loopcl", 0x92, ARGS1(kAF_Offset16) },

{ "jmpl", 0x72,  ARGS1(kAF_Jump16)},
{ "chkl", 0x95, ARGS2(kAF_Condition, kAF_Offset16) },
{ "chk", 0x85, ARGS2(kAF_Condition, kAF_Offset8) },
{ "jmp", 0x27, ARGS1(kAF_Jump8) },

// opcodes that are primarily for Direct mode
{ "ping", 0x10, 0 },
{ "poll", 0x12, ARGS1(kAF_Value8) },
{ "pollp", 0x15, ARGS5(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "memmap", 0x20, 0 },
{ "_task", 0x25, ARGS4(kAF_Skip8, kAF_Raw8, kAF_Skip8, kAF_Raw16) },
{ "_sub", 0x35, ARGS4(kAF_Skip8, kAF_Raw8, kAF_Skip8, kAF_Raw16) },
{ "_firm", 0x75, ARGS3(kAF_Raw16, kAF_Raw16, kAF_Skip8) },
{ "pollb", 0x30, 0},
{ "delt", 0x40, 0},
{ "delt", 0x61, ARGS1(kAF_Raw8) },
{ "dels", 0x70, 0},
{ "dels", 0xc1, ARGS1(kAF_Raw8)},
{ "prgm", 0x91, ARGS1(kAF_Raw8) },
{ "boot", 0xa5, ARGS5(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "tout", 0xb1, ARGS1(kAF_Raw8) },

// Added for Scout
{ "cntd", 0xa7, ARGS1(kAF_Raw8) },
{ "cnti", 0x97, ARGS1(kAF_Raw8) },
{ "cntz", 0xb7, ARGS1(kAF_Raw8) },

{ "decvjn", 0xf2, ARGS2(kAF_Var, kAF_Jump8) },
{ "decvjnl", 0xf3, ARGS2(kAF_Var, kAF_Jump16) },

{ "event", 0x03, ARGS1(kAF_Value16) },
{ "mone", 0xb4, ARGS2(kAF_Value16, kAF_Jump8) },
{ "monel", 0xb5, ARGS2(kAF_Value16, kAF_Jump16) },
{ "monex", 0xb0, 0 },

{ "monal", 0x73, ARGS2(kAF_Raw8, kAF_Jump16) },
{ "monax", 0xa0, 0 },

{ "gdir", 0x77, ARGS1(kAF_OutputDir) },
{ "gout", 0x67, ARGS1(kAF_OutputMode) },
{ "gpwr", 0xa3, ARGS2(kAF_Outputs, kAF_Value8) },

{ "playv", 0x02, ARGS2(kAF_Var, kAF_Raw8) },
{ "pollm", 0x63, ARGS2(kAF_Raw16, kAF_Raw8) },
{ "remote", 0xd2, ARGS1(kAF_Raw16) },
{ "setp", 0xd7, ARGS1(kAF_Raw8) },
{ "rets", 0xf6, 0 },

// new for RCX 2.0
{ "disp", 0xe5, ARGS3(kAF_Skip8, kAF_Raw8, kAF_Value16) },
{ "playz", 0x80, 0},
{ "uart", 0xc2, ARGS2(kAF_Raw8, kAF_Raw8) },
{ "set", 0x05, ARGS2(kAF_Value8, kAF_Value16) },
{ "dele", 0x06, 0 },
{ "sete", 0x93, ARGS3(kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "mute", 0xd0, 0 },
{ "speak", 0xe0, 0 },
{ "cale", 0x04, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "msgs", 0xf7, ARGS1(kAF_Raw8) },

{0} // termination
};


// custom Cybermaster instructions
static RCX_Disasm::Instruction sCMInstructions[] = {
{ "Drive", 0x41, ARGS1(kAF_Raw8) },
{ "OnWait", 0xc2, ARGS2(kAF_Raw8, kAF_Raw8) },
{ "OnWaitDifferent", 0x53, ARGS3(kAF_Raw8,  kAF_Raw8, kAF_Raw8) },
{ "ClearTacho", 0x11, ARGS1(kAF_Raw8) },
{0} // termination
};


// custom Scout instructions
static RCX_Disasm::Instruction sScoutInstructions[] = {
{ "scout", 0x47, ARGS1(kAF_Raw8) },
{ "sound", 0x57, ARGS1(kAF_Raw8) },
{ "setfb", 0x83, ARGS1(kAF_Value16) },
{ "light", 0x87, ARGS1(kAF_Raw8) },
{ "vll", 0xe2, ARGS1(kAF_Value8) },
{ "lsut", 0xb3, ARGS1(kAF_Value16) },
{ "lscal", 0xc0, 0 },
{ "lslt", 0xc3, ARGS1(kAF_Value16) },
{ "lsbt", 0xe3, ARGS1(kAF_Value16) },
{ "lsh", 0xd3, ARGS1(kAF_Value16) },
{ "tmrs", 0xc4, ARGS2(kAF_Raw8, kAF_Value16) },
{ "cnts", 0xd4, ARGS2(kAF_Raw8, kAF_Value16) },
{ "rules", 0xd5, ARGS5(kAF_Raw8,kAF_Raw8,kAF_Raw8,kAF_Raw8,kAF_Raw8) },
{0} // termination
};


// custom Spybotics instructions
static RCX_Disasm::Instruction sSpyboticsInstructions[] = {
{ "playm", 0xc7, ARGS1(kAF_Raw8) },
{ "playsv", 0xe7, ARGS1(kAF_Var) },
{ "pop", 0x01, ARGS1(kAF_Raw8) },
{ "relz", 0x36, 0},
{ "find", 0xd5, ARGS2(kAF_Var, kAF_SrcRelThresh) },
{ "polle", 0x11, ARGS1(kAF_Raw8) },
{ "push", 0xe3, ARGS1(kAF_Value16) },
{ "light", 0x87, ARGS1(kAF_Raw8) },
{ "vll", 0xe2, ARGS1(kAF_Value8) },
{0} // termination
};

static RCX_Disasm::Instruction sSpyboticsResourceInstructions[] = {
{ "freqs", 0x45, ARGS3(kAF_Raw8, kAF_Raw16, kAF_Raw16) },
{ "freqv", 0x55, ARGS3(kAF_Raw8, kAF_Raw16, kAF_Raw16) },
{ "pausef", 0x81, ARGS1(kAF_Raw8) },
{ "pauses", 0x21, ARGS1(kAF_Raw8) },
{ "repeats", 0x10, 0 },
{ "startg", 0x62, ARGS2(kAF_Raw8, kAF_Raw8) },
{ "stopg", 0x70, 0 },
{ "stops", 0x00, 0 },
{ "tonef", 0x93, ARGS2(kAF_Raw8, kAF_Raw16) },
{ "tones", 0x33, ARGS2(kAF_Raw8, kAF_Raw16) },
{0} // termination
};


// custom Swan instructions
static RCX_Disasm::Instruction sSwanInstructions[] = {
{ "noop", 0x00, 0 },
//{ "dgets", 0x01, ARGS1(kAF_Raw8) },
//{ "dunrp", 0x07, ARGS1(kAF_Raw8) },
{ "sum", 0x08, ARGS2(kAF_Value16, kAF_Value16) },
{ "sub", 0x09, ARGS2(kAF_Value16, kAF_Value16) },
{ "mul", 0x0a, ARGS2(kAF_Value16, kAF_Value16) },
{ "div", 0x0b, ARGS2(kAF_Value16, kAF_Value16) },
{ "and", 0x0c, ARGS2(kAF_Value16, kAF_Value16) },
{ "or" , 0x0d, ARGS2(kAF_Value16, kAF_Value16) },
{ "not", 0x0e, ARGS2(kAF_Value16, kAF_Value16) },
{ "mod", 0x0f, ARGS2(kAF_Value16, kAF_Value16) },
//{ "setsvbc", 0x11, ARGS2(kAF_Raw16, kAF_Raw8) },
//{ "dclrex", 0x16, 0 },
{ "suml", 0x18, ARGS2(kAF_Value16, kAF_Value16) },
{ "subl", 0x19, ARGS2(kAF_Value16, kAF_Value16) },
{ "mull", 0x1a, ARGS2(kAF_Value16, kAF_Value16) },
{ "divl", 0x1b, ARGS2(kAF_Value16, kAF_Value16) },
{ "andl", 0x1c, ARGS2(kAF_Value16, kAF_Value16) },
{ "orl" , 0x1d, ARGS2(kAF_Value16, kAF_Value16) },
{ "notl", 0x1e, ARGS2(kAF_Value16, kAF_Value16) },
{ "modl", 0x1f, ARGS2(kAF_Value16, kAF_Value16) },
{ "setl", 0x26, ARGS2(kAF_Value16, kAF_Value16) },
{ "setlw", 0x28, ARGS2(kAF_Value16, kAF_HLRaw16) },
{ "setll", 0x29, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "arrayb", 0x2a, ARGS3(kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "setgb", 0x2b, ARGS2(kAF_GVar, kAF_Raw8) },
{ "setgw", 0x2c, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "setg", 0x2d, ARGS2(kAF_GVar, kAF_Value8) },
{ "notv", 0x2e, ARGS2(kAF_Var, kAF_Value16) },
{ "setsv", 0x2f, ARGS2(kAF_Value16, kAF_Value16) },
{ "sets", 0x36, ARGS2(kAF_Value8, kAF_Value8) },
{ "sumf", 0x38, ARGS2(kAF_Value16, kAF_Value16) },
{ "subf", 0x39, ARGS2(kAF_Value16, kAF_Value16) },
{ "mulf", 0x3a, ARGS2(kAF_Value16, kAF_Value16) },
{ "divf", 0x3b, ARGS2(kAF_Value16, kAF_Value16) },
{ "tablbn", 0x3c, ARGS2(kAF_CondV2V2, kAF_Offset8) },
{ "tablbf" , 0x3d, ARGS2(kAF_CondV2V2, kAF_HLOffset16) },
{ "tabibf", 0x3e, ARGS2(kAF_CondV2V2, kAF_HLOffset16) },
{ "tabfbf", 0x3f, ARGS2(kAF_CondV2V2, kAF_HLOffset16) },
//{ "send message with 1 parm byte", 0x41, 0 },
{ "upevtmap", 0x46, 0 },
{ "waitms", 0x47, ARGS1(kAF_Value16) },
{ "sumgb", 0x48, ARGS2(kAF_GVar, kAF_Raw8) },
{ "subgb", 0x49, ARGS2(kAF_GVar, kAF_Raw8) },
{ "mulgb", 0x4a, ARGS2(kAF_GVar, kAF_Raw8) },
{ "divgb", 0x4b, ARGS2(kAF_GVar, kAF_Raw8) },
{ "andgb", 0x4c, ARGS2(kAF_GVar, kAF_Raw8) },
{ "orgb" , 0x4d, ARGS2(kAF_GVar, kAF_Raw8) },
{ "notgb", 0x4e, ARGS2(kAF_GVar, kAF_Raw8) },
{ "modgb", 0x4f, ARGS2(kAF_GVar, kAF_Raw8) },
//{ "brnb", 0x53, ARGS1(kAF_Offset8) },
//{ "brfw", 0x55, ARGS1(kAF_Offset16) },
{ "tabicsn", 0x56, ARGS2(kAF_CondC1V1, kAF_Offset8) },
{ "tabicbn", 0x57, ARGS2(kAF_CondC2V2, kAF_Offset8) },
{ "sumgw", 0x58, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "subgw", 0x59, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "mulgw", 0x5a, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "divgw", 0x5b, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "andgw", 0x5c, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "orgw" , 0x5d, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "notgw", 0x5e, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "modgw", 0x5f, ARGS2(kAF_GVar, kAF_HLRaw16) },
{ "assert", 0x66, 0 },
{ "sumll", 0x68, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "subll", 0x69, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "mulll", 0x6a, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "divll", 0x6b, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "andll", 0x6c, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "orll" , 0x6d, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "notll", 0x6e, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "modll", 0x6f, ARGS5(kAF_Value16, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "setlb", 0x76, ARGS2(kAF_Value16, kAF_Raw8) },
{ "tabiscn", 0x78, ARGS2(kAF_CondV1C1, kAF_Offset8) },
{ "tabibcn", 0x79, ARGS2(kAF_CondV2C2, kAF_Offset8) },
//{ "waittv", 0x7a, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "abs", 0x7b, ARGS2(kAF_Value16, kAF_Value16) },
{ "sgn", 0x7c, ARGS2(kAF_Value16, kAF_Value16) },
{ "btst", 0x7d, ARGS2(kAF_Value16, kAF_Value16) },
{ "bset", 0x7e, ARGS2(kAF_Value16, kAF_Value16) },
{ "bclr", 0x7f, ARGS2(kAF_Value16, kAF_Value16) },
//{ "upg", 0x83, ARGS3(kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "tabfsf", 0x86, ARGS2(kAF_CondV1V1, kAF_HLOffset16) },
//{ "upevent", 0x87, ARGS1(kAF_Raw8) },
{ "xor", 0x88, ARGS2(kAF_Value16, kAF_Value16) },
{ "tabfbn", 0x89, ARGS2(kAF_CondV2V2, kAF_Offset8) },
{ "tabfsn", 0x8a, ARGS2(kAF_CondV1V1, kAF_Offset8) },
{ "tablsn", 0x8b, ARGS2(kAF_CondV1V1, kAF_Offset8) },
{ "ftol", 0x8c, ARGS2(kAF_Value16, kAF_Value16) },
{ "itol", 0x8d, ARGS2(kAF_Value16, kAF_Value16) },
{ "tabisn", 0x8e, ARGS2(kAF_CondV1V1, kAF_Offset8) },
{ "tabibn", 0x8f, ARGS2(kAF_CondV2V2, kAF_Offset8) },
{ "tablsf", 0x96, ARGS2(kAF_CondV1V1, kAF_HLOffset16) },
{ "shl", 0x98, ARGS2(kAF_Value16, kAF_Value16) },
{ "shr", 0x99, ARGS2(kAF_Value16, kAF_Value16) },
//{ "trimulc", 0x9a, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "neg", 0x9b, ARGS2(kAF_Value16, kAF_Value16) },
//{ "swchitn", 0x9e, 0 },
//{ "swchitf", 0x9f, 0 },
{ "msgsb", 0xa2, ARGS2(kAF_Raw8, kAF_Raw8) },
//{ "trisubc", 0xa6, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "trisum", 0xa8, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "trisub", 0xa9, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "trimul", 0xaa, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "trisumc", 0xab, ARGS6(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "itof", 0xac, ARGS2(kAF_Value16, kAF_Value16) },
{ "ltof", 0xad, ARGS2(kAF_Value16, kAF_Value16) },
{ "ftoi", 0xae, ARGS2(kAF_Value16, kAF_Value16) },
{ "ltoi", 0xaf, ARGS2(kAF_Value16, kAF_Value16) },
{ "msgsw", 0xb3, ARGS2(kAF_Raw8, kAF_Raw16) },
{ "uptimer", 0xb6, 0 },
{ "sums", 0xb8, ARGS2(kAF_Value8, kAF_Value8) },
{ "subs", 0xb9, ARGS2(kAF_Value8, kAF_Value8) },
{ "muls", 0xba, ARGS2(kAF_Value8, kAF_Value8) },
{ "divs", 0xbb, ARGS2(kAF_Value8, kAF_Value8) },
{ "ands", 0xbc, ARGS2(kAF_Value8, kAF_Value8) },
{ "ors" , 0xbd, ARGS2(kAF_Value8, kAF_Value8) },
{ "nots", 0xbe, ARGS2(kAF_Value8, kAF_Value8) },
{ "mods", 0xbf, ARGS2(kAF_Value8, kAF_Value8) },
{ "msgp", 0xc3, ARGS2(kAF_Value8, kAF_Value16) },
{ "msgsv", 0xc4, ARGS2(kAF_Value8, kAF_Value16) },
//{ "swch", 0xc5, 0 },
//{ "swchbc", 0xc6, 0 },
{ "clrall", 0xc7, ARGS1(kAF_Raw8) },
{ "sumg", 0xc8, ARGS2(kAF_GVar, kAF_Value8) },
{ "subg", 0xc9, ARGS2(kAF_GVar, kAF_Value8) },
{ "mulg", 0xca, ARGS2(kAF_GVar, kAF_Value8) },
{ "divg", 0xcb, ARGS2(kAF_GVar, kAF_Value8) },
{ "andg", 0xcc, ARGS2(kAF_GVar, kAF_Value8) },
{ "org" , 0xcd, ARGS2(kAF_GVar, kAF_Value8) },
{ "notg", 0xce, ARGS2(kAF_GVar, kAF_Value8) },
{ "modg", 0xcf, ARGS2(kAF_GVar, kAF_Value8) },
{ "playkhz", 0xd3, ARGS1(kAF_Value16) },
//{ "getl", 0xd4, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "tabisf", 0xd5, ARGS2(kAF_CondV1V1, kAF_HLOffset16) },
{ "updevvar", 0xd6, 0 },
{ "sumsw", 0xd8, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "subsw", 0xd9, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "mulsw", 0xda, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "divsw", 0xdb, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "andsw", 0xdc, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "orsw" , 0xdd, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "notsw", 0xde, ARGS2(kAF_Value8, kAF_HLRaw16) },
{ "modsw", 0xdf, ARGS2(kAF_Value8, kAF_HLRaw16) },
//{ "dbgsrs", 0xe2, ARGS2(kAF_Raw8, kAF_Raw8) },
{ "playkhzv", 0xe3, ARGS1(kAF_Value16) },
//{ "dbgsbrk", 0xe4, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "brk0", 0xe6, 0 },
{ "brk1", 0xe7, 0 },
{ "brk2", 0xe8, 0 },
{ "brk3", 0xe9, 0 },
{ "brk4", 0xea, 0 },
{ "brk5", 0xeb, 0 },
{ "brk6", 0xec, 0 },
{ "brk7", 0xed, 0 },
{ "brk8", 0xee, 0 },
{ "brk9", 0xef, 0 },
//{ "motors", 0xf1, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
//{ "dbgspc", 0xf4, ARGS4(kAF_Raw8, kAF_Raw8, kAF_Raw8, kAF_Raw8) },
{ "modv", 0xf5, ARGS2(kAF_Var, kAF_Value16) },
{ "sump", 0xf8, ARGS2(kAF_Value16, kAF_Value16) },
{ "subp", 0xf9, ARGS2(kAF_Value16, kAF_Value16) },
{ "mulp", 0xfa, ARGS2(kAF_Value16, kAF_Value16) },
{ "divp", 0xfb, ARGS2(kAF_Value16, kAF_Value16) },
{ "andp", 0xfc, ARGS2(kAF_Value16, kAF_Value16) },
{ "orp" , 0xfd, ARGS2(kAF_Value16, kAF_Value16) },
{ "notp", 0xfe, ARGS2(kAF_Value16, kAF_Value16) },
{ "modp", 0xff, ARGS2(kAF_Value16, kAF_Value16) },
{0} // termination
};

class SrcListState
{
public:
	bool	fEnabled;
	short	fIndex;
	long	fOffset;
};

// Swan support code
const ubyte nSwanInstructionLength[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

ubyte getSwanInstructionLength(const ubyte anOpcode)
{
  return nSwanInstructionLength[(ubyte) anOpcode];
}

ubyte getSwanArgLength(const ubyte anOpcode)
{
  return nSwanInstructionLength[(ubyte) anOpcode] - 1;
}

typedef unsigned short uword;
typedef ubyte * pUbyte;

void calculateInstructionLengths()
{
  typedef struct
  {
    ubyte theOpcode;
    ubyte  theLength;
  } TOpcodeLengths;

  {
    uword index; // don't use ubyte or will get screwed up on range check '< 256'
    //
    // initialize instruction length array.
    //
    for (index = 0; index < 256; ++index)
    {
      //
      // instruction length is stored in an array rather than calculating every time we
      // go through 'execute one instruction' loop.
      // This is [1] for improved real time performance and [2] because no simple formula
      // to calculate length for many of the new opcodes.
      //
      register ubyte nBytesInOpcode;
      register pUbyte ptr;

      nBytesInOpcode = (ubyte) (index & 0x07); // actually one less than the value
      if (nBytesInOpcode > 5)
        nBytesInOpcode -= 5;
      else
        ++nBytesInOpcode;
      //
      // Note:
      // rcxTestAndBranchFar [8 bytes] and rcxTestAndBranchNear [7] are more than 5 bytes.
      // As well as many of the new opcodes
      //
      //
      ptr = (pUbyte) &nSwanInstructionLength[index];
      *ptr = nBytesInOpcode;
    }
  }
  //
  // special case instructions. Don't follow the above formula.
  //
  {
    static const TOpcodeLengths OpcodeLengths[] =
    {
      {(ubyte) rcxBreakpoint0,              1},
      {(ubyte) rcxBreakpoint1,              1},
      {(ubyte) rcxBreakpoint2,              1},
      {(ubyte) rcxBreakpoint3,              1},
      {(ubyte) rcxBreakpoint4,              1},
      {(ubyte) rcxBreakpoint5,              1},
      {(ubyte) rcxBreakpoint6,              1},
      {(ubyte) rcxBreakpoint7,              1},
      {(ubyte) rcxBreakpoint8,              1},
      {(ubyte) rcxBreakpoint9,              1},

      {(ubyte) rcxSetSourceValueByteConst,  5},
      {(ubyte) rcxSetSourceValueShortVariable, 5},
      {(ubyte) rcxAssignSourceValue,        7},

      {(ubyte) rcxAssignGlobalConstantByte, 3},

      {(ubyte) rcxSetMotorState,            5},

      {(ubyte) rcxAssignGlobalConstant,     4},
      {(ubyte) rcxAssignGlobalVariable,     4},

      {(ubyte) rcxArrayBounds,              4},
      {(ubyte) rcxAssert,                   1},

      {(ubyte) rcxWaitTimer1MSec,           4},

      {(ubyte) rcxBitComplement,            5},
      {(ubyte) rcxModuloTo,                 5},

      {(ubyte) rcxWaitTimerValue,           5},

      {(ubyte) rcxTrinaryAdd,               7},
      {(ubyte) rcxTrinaryMinus,             7},
      {(ubyte) rcxTrinaryTimes,             7},
      {(ubyte) rcxTrinaryAddConstant,       7},
      {(ubyte) rcxTrinaryMinusConstant,     7},
      {(ubyte) rcxTrinaryTimesConstant,     7},

      {(ubyte) rcxAbsoluteValueSourceValue, 7},
      {(ubyte) rcxSignValueSourceValue,     7},

      {(ubyte) rcxBranchNearByte,           2},
      {(ubyte) rcxBranchFarWord,            3},

      {(ubyte) rcxSwitch,                   7}, // will recalculate on fly
      {(ubyte) rcxSwitchByteCase,           7}, // will recalculate on fly
      {(ubyte) rcxSwitchIndexTableFar,      7}, // will recalculate on fly
      {(ubyte) rcxSwitchIndexTableNear,     7}, // will recalculate on fly

      {(ubyte) rcxTestAndBranchNear,        7},
      {(ubyte) rcxTestAndBranchFar,         8},

      {(ubyte) rcxTestAndBranchIntShortFar, 7},
      {(ubyte) rcxTestAndBranchIntBigFar,   9},
      {(ubyte) rcxTestAndBranchIntConstShortNear,6},
      {(ubyte) rcxTestAndBranchIntConstBigNear,  8},
      {(ubyte) rcxTestAndBranchIntShortConstNear,6},
      {(ubyte) rcxTestAndBranchIntBigConstNear,  8},
      {(ubyte) rcxTestAndBranchIntShortNear,6},
      {(ubyte) rcxTestAndBranchIntBigNear,  8},

      {(ubyte) rcxTestAndBranchLongShortFar,7},
      {(ubyte) rcxTestAndBranchLongBigFar,  9},
      {(ubyte) rcxTestAndBranchLongShortNear,6},
      {(ubyte) rcxTestAndBranchLongBigNear, 8},

      {(ubyte) rcxTestAndBranchFloatShortFar,7},
      {(ubyte) rcxTestAndBranchFloatBigFar, 9},
      {(ubyte) rcxTestAndBranchFloatShortNear,6},
      {(ubyte) rcxTestAndBranchFloatBigNear,8},

      {(ubyte) rcxIntegerToLong,            7},
      {(ubyte) rcxIntegerToFloat,           7},

      {(ubyte) rcxFloatToLong,              7},
      {(ubyte) rcxFloatToInteger,           7},

      {(ubyte) rcxLongToFloat,              7},
      {(ubyte) rcxLongToInteger,            7},

      {(ubyte) rcxAddToFloat,               7},
      {(ubyte) rcxMinusToFloat,             7},
      {(ubyte) rcxTimesToFloat,             7},
      {(ubyte) rcxDivideToFloat,            7},

      {(ubyte) rcxAssignLong,               7},

      {(ubyte) rcxAssignLongConstant,       8},
      {(ubyte) rcxAssignLongConstantWord,   6},
      {(ubyte) rcxAssignLongConstantByte,   5},

      {(ubyte) rcxBitTest,                  7},
      {(ubyte) rcxBitSet,                   7},
      {(ubyte) rcxBitClear,                 7},
      {(ubyte) rcxXOr,                      7},
      {(ubyte) rcxShiftLeft,                7},
      {(ubyte) rcxShiftRight,               7},
      {(ubyte) rcxNegate,                   7},

      {(ubyte) rcxSendMessageWithParm,      6},
      {(ubyte) rcxSetMessageVariableParm,   6},

      {(ubyte) 0,                           0}
    };

    register ubyte index;

    for (index = 0; ; ++index)
    {
      register TRCXop theOpcode;

      theOpcode = (TRCXop) OpcodeLengths[index].theOpcode;
      if (theOpcode == (TRCXop) 0)
        break;
      * (pUbyte) &nSwanInstructionLength[(ubyte) theOpcode] = OpcodeLengths[index].theLength;
    }
  }

  //
  // Ints and longs have more arithmetic: +, -, *, /. &, |, ~, %
  //
  {
    static const TOpcodeLengths ArithLengths[] =
    {
      {(ubyte) rcxAddToGlobal,            4},
      {(ubyte) rcxAddToGlobalUbyte,       3},
      {(ubyte) rcxAddToGlobalConstant,    4},
      {(ubyte) rcxAddToSourceValue,       7},
      {(ubyte) rcxAddToGenericParms,      7},

      {(ubyte) rcxAddToShortVar,          5},
      {(ubyte) rcxAddToShortVarConstant,  5},

      {(ubyte) rcxAddToLong,              7},
      {(ubyte) rcxAddToLongConstant,      8},
      {(ubyte) 0,                         0}
    };

    register ubyte index;

    for (index = 0;  ; ++index)
    {
      register TRCXop theOpcode;
      register ubyte  theLength;
      register ubyte idx;

      theOpcode = (TRCXop) ArithLengths[index].theOpcode;
      if (theOpcode == (TRCXop) 0)
        break;
      theLength = ArithLengths[index].theLength;
      for (idx = arithOffsetAddTo; idx <= arithOffsetModuloTo; ++idx)
      {
        * (pUbyte) &nSwanInstructionLength[(ubyte) (((ubyte) theOpcode) + (ubyte) idx)] = theLength;
      }
    }
  }
}




RCX_Disasm::RCX_Disasm(RCX_TargetType targetType) : fTarget(targetType)
{
	unsigned i;

	for(i=0; i<256; ++i) {
		fOpDispatch[i] = 0;
	}

	for(i=0; i<256; ++i) {
	        fResOpDisp[i] = 0;
	}

        if (targetType == kRCX_SwanTarget)
          calculateInstructionLengths();

	DefineInstructions(fOpDispatch, sGenericInstructions);

	switch(targetType) {
		case kRCX_CMTarget:
			DefineInstructions(fOpDispatch, sCMInstructions);
			break;

		case kRCX_ScoutTarget:
			DefineInstructions(fOpDispatch, sScoutInstructions);
			break;

		case kRCX_SpyboticsTarget:
			DefineInstructions(fOpDispatch, sSpyboticsInstructions);
			DefineInstructions(fResOpDisp, sSpyboticsResourceInstructions);
			break;

                case kRCX_SwanTarget:
                        DefineInstructions(fOpDispatch, sSwanInstructions);
                        break;

		default:
			break;
	}
}


void RCX_Disasm::LASMOutputHeader(RCX_Printer *dst, string name, RCX_ChunkType type, int ChunkNum)
{
  char line[256];
  char typeName[10];
  switch(type)
  {
    case kRCX_TaskChunk  : strcpy(typeName, "task"); break;
    case kRCX_SubChunk   : strcpy(typeName, "sub"); break;
    case kRCX_SoundChunk : strcpy(typeName, "sound"); break;
    case kRCX_AnimationChunk : strcpy(typeName, "mood"); break;
    default: break;
//    case kRCX_DataChunk : strcpy(typeName, "data"); break;
  }
  sprintf(line, ";%s\n\t%s\t%d\n", name.c_str(), typeName, ChunkNum);
  dst->Print(line);
}

void RCX_Disasm::LASMOutputFooter(RCX_Printer *dst, RCX_ChunkType type, UShort pc)
{
  char line[256];

  // output the last label
  labels_type::iterator p = fLabels.find((int)pc);
  if (p != fLabels.end())
  {
      string tmp = (*p).second;
      sprintf(line, "%s:\n", tmp.c_str());
      dst->Print(line);
  }

  char typeName[10];
  switch(type)
  {
    case kRCX_TaskChunk  : strcpy(typeName, "endt"); break;
    case kRCX_SubChunk   :
    case kRCX_SoundChunk : strcpy(typeName, "ends"); break;
    case kRCX_AnimationChunk : strcpy(typeName, "endm"); break;
    default: break;
//    case kRCX_DataChunk : strcpy(typeName, "endd"); break;
  }
  sprintf(line, "\t%s\n", typeName);
  dst->Print(line);
}

void RCX_Disasm::Print(RCX_Printer *dst, bool genLASM, string name,
  RCX_ChunkType type, int ChunkNum, const UByte *code, int length)
{
	Print(dst, genLASM, name, type, ChunkNum, code, length, 0, 0, 0);
}

void RCX_Disasm::Print(RCX_Printer *dst, bool genLASM, string name,
  RCX_ChunkType type, int ChunkNum, const UByte *code, int length,
  RCX_SourceFiles *sf, const RCX_SourceTag *tags, int tagCount)
{
        fGenLASM = genLASM;
        fCurType = type;
        fCurNum  = ChunkNum;

	UShort pc = 0;
        int tmpLen = length;
        const UByte *tmpCode = code;
	RCX_Result result;
	stack<SrcListState> stateStack;

        // start with an empty set of labels
        fLabels.clear();

	// ignore tags if no source files
	if (!sf) tagCount = 0;

//	if (genLASM) tagCount = 0; // no source listing in LASM compatible output

	if (genLASM)
        {
          LASMOutputHeader(dst, name, type, ChunkNum);

          // run through the entire chunk once to get a list of required labels
          while(true)
          {
              if (tmpLen <= 0) break;

              result = FindLabel(tmpCode, tmpLen, pc);
              pc += result;
              tmpCode += result;
              tmpLen -= result;
          }
        }

        pc = 0;
        tmpLen = length;
        tmpCode = code;

        // now do it all again to print the listing

        while(true)
        {
            // tag processing happens before length check to allow
            // fragment end tags (which have a pc at the end of
            // the fragment) to be processed
            while (tagCount && (tags->fAddress <= pc) )
            {
                    switch(tags->fType)
                    {
                            case RCX_SourceTag::kBegin:
                            case RCX_SourceTag::kBeginNoList:
                            {
                                    SrcListState ns;
                                    ns.fEnabled = (tags->fType == RCX_SourceTag::kBegin);
                                    ns.fIndex = tags->fSrcIndex;
                                    ns.fOffset = tags->fSrcOffset;
                                    stateStack.push(ns);
                                    break;
                            }
                            case RCX_SourceTag::kNormal:
                            {
                                    if (stateStack.empty()) break;
                                    SrcListState &s = stateStack.top();
                                    if (tags->fSrcIndex != s.fIndex) break;
                                    if (s.fEnabled)
                                            s.fOffset = sf->Print(dst, s.fIndex, s.fOffset, tags->fSrcOffset) + 1;
                                    break;
                            }
                            case RCX_SourceTag::kEnd:
                            {
                                    if (stateStack.empty()) break;
                                    SrcListState &s = stateStack.top();
                                    if (s.fEnabled)
                                            sf->Print(dst, s.fIndex, s.fOffset, tags->fSrcOffset);
                                    stateStack.pop();
                                    break;
                            }
                    }

                    tags++;
                    tagCount--;
            }

            if (tmpLen <= 0) break;

            result = Print1(dst, tmpCode, tmpLen, pc);
            pc += result;
            tmpCode += result;
            tmpLen -= result;
        }
        if (genLASM)
          LASMOutputFooter(dst, type, pc);
}


RCX_Result RCX_Disasm::Print1(RCX_Printer *dst, const UByte *code, int length, UShort pc)
{
	char text[256];
	char line[256];

	RCX_Result result = SPrint1(text, code, length, pc);

	if (result < 1)
	{
		result = 1;
		sprintf(text, "?");
	}

        if (fGenLASM)
        {
            if (fCurType == kRCX_SoundChunk)
                sprintf(line, "\t%s ", text);
            else
            {
                // does this line have a label???
                labels_type::iterator p = fLabels.find((int)pc);
                if (p != fLabels.end())
                {
                    string tmp = (*p).second;
                    sprintf(line, "%s:\n\t%s ", tmp.c_str(), text);
                }
                else
                    sprintf(line, "\t%s ", text);
            }
        }
        else
	    sprintf(line, "%03d %-42s ", pc, text);
	dst->Print(line);

        if (!fGenLASM)
        {
            for(int i=0; i<result; i++)
            {
                    sprintf(line, "%02x ", code[i]);
                    dst->Print(line);
            }
        }

      	sprintf(line, "\n");
	dst->Print(line);


	return result;
}



void RCX_Disasm::SPrintArg(char *text, ULong format, const UByte *code, UShort pc)
{
	char buf[32];

	switch(format)
	{
		case kAF_Raw8:
			sprintf(text, "%d", code[0]);
			break;
		case kAF_Raw16:
			sprintf(text, "%d", (short)WORD(code));
			break;
		case kAF_Value8:
			SPrintValue(text, code[0], code[1]);
			break;
		case kAF_Value16:
			SPrintValue(text, code[0], (short)WORD(code+1));
			break;
		case kAF_Var:
			if (fGenLASM)
				sprintf(text, "%d", code[0]);
			else
				sprintf(text, "var[%d]", code[0]);
			break;
		case kAF_SrcRelThresh:
			if (fGenLASM)
				sprintf(text, "%d, %d, %d, %d", code[0], (code[1] >> 6) & 3, code[1] & 0x3f, (short)WORD(code+2));
			else
                        {
				SPrintValue(buf, code[1] & 0x3f, (short)WORD(code+2));
				sprintf(text, "%s %s %s", GetTypeName(code[0]), LOOKUP((code[1] >> 6) & 3, findRelNames), buf);
                        }
			break;
		case kAF_Outputs:
                        if (fGenLASM)
                          sprintf(text, "%d", code[0] & 7);
                        else
			  SPrintOutputNames(text, code[0] & 7);
			break;
		case kAF_OutputMode:
			if (fGenLASM)
				sprintf(text, "%d, %d", code[0] & 7, (code[0] >> 6) & 3);
			else
			{
				SPrintOutputNames(buf, code[0] & 7);
				sprintf(text, "%s, %s", buf, LOOKUP((code[0] >> 6) & 3, outputModeName));
			}
			break;
		case kAF_OutputDir:
                        if (fGenLASM)
                          sprintf(text, "%d, %d", (code[0] >> 6) & 3, code[0] & 7);
                        else
                        {
                          SPrintOutputNames(buf, code[0] & 7);
                          sprintf(text, "%s, %s", buf, LOOKUP((code[0] >> 6) & 3, outputDirName));
                        }
			break;
		case kAF_InputType:
                        if (fGenLASM)
                          sprintf(text, "%d", code[0]);
                        else
			  sprintf(text, "%s", LOOKUP(code[0], inputTypeNames));
			break;
		case kAF_InputMode:
                        if (fGenLASM)
                          sprintf(text, "%d, %d", (code[0] >> 5) & 7, (code[0] & 31));
                        else
			  sprintf(text, "%s", LOOKUP((code[0] >> 5) & 7, inputModeNames));
			break;
		case kAF_Condition:
			SPrintCondition(text, code);
			break;
		case kAF_Jump8:
                        if (fGenLASM)
                        {
                            labels_type::iterator p = fLabels.find((int)(pc + ComputeOffset(code[0], 0)));
                            if (p != fLabels.end())
                            {
                                string tmp = (*p).second;
                                sprintf(text, "%s", tmp.c_str());
                            }
                            else
                                sprintf(text, "%d", pc + ComputeOffset(code[0], 0));
                        }
                        else
			    sprintf(text, "%d", pc + ComputeOffset(code[0], 0));
			break;
		case kAF_Jump16:
                        if (fGenLASM)
                        {
                            labels_type::iterator p = fLabels.find((int)(pc + ComputeOffset(code[0], code[1])));
                            if (p != fLabels.end())
                            {
                                string tmp = (*p).second;
                                sprintf(text, "%s", tmp.c_str());
                            }
                            else
                                sprintf(text, "%d", pc + ComputeOffset(code[0], code[1]));
                        }
                        else
                            sprintf(text, "%d", pc + ComputeOffset(code[0], code[1]));
			break;
		case kAF_Offset8:
                        if (fGenLASM)
                        {
                            labels_type::iterator p = fLabels.find((int)(pc + (signed char)code[0]));
                            if (p != fLabels.end())
                            {
                                string tmp = (*p).second;
                                sprintf(text, "%s", tmp.c_str());
                            }
                            else
                                sprintf(text, "%d", pc + (signed char)code[0]);
                        }
                        else
			    sprintf(text, "%d", pc + (signed char)code[0]);
			break;
		case kAF_Offset16:
                        if (fGenLASM)
                        {
                            labels_type::iterator p = fLabels.find((int)(pc + (short)WORD(code)));
                            if (p != fLabels.end())
                            {
                                string tmp = (*p).second;
                                sprintf(text, "%s", tmp.c_str());
                            }
                            else
                                sprintf(text, "%d", pc + (short)WORD(code));
                        }
                        else
			    sprintf(text, "%d", pc + (short)WORD(code));
			break;
                case kAF_GVar:
                        if (fGenLASM)
                          sprintf(text, "%d", code[0]);
                        else
                          sprintf(text, "gvar[%d]", code[0]);
                        break;
                case kAF_HLRaw16:
                        sprintf(text, "%d", (short)HLWORD(code));
                        break;
		case kAF_None:
		case kAF_Skip8:
		case kAF_Skip16:
		default:
			text[0] = 0;
			break;
	}
}

bool ResourceType(RCX_ChunkType type)
{
  return type == kRCX_SoundChunk || type == kRCX_AnimationChunk;
}

void RCX_Disasm::FindLabelArg(ULong format, const UByte *code, UShort pc)
{
	char text[256];
	char buf[256];
	int value;
	switch(format)
	{
		case kAF_Jump8:
			value = pc + ComputeOffset(code[0], 0);
			break;
		case kAF_Jump16:
			value = pc + ComputeOffset(code[0], code[1]);
			break;
		case kAF_Offset8:
			value = pc + (signed char)code[0];
			break;
		case kAF_Offset16:
			value = pc + (short)WORD(code);
			break;
		default:
			value = -1;
			break;
	}

	if (value != -1)
	{
		switch(fCurType)
		{
			case kRCX_TaskChunk : sprintf(buf, "t%03d", fCurNum); break;
			case kRCX_SubChunk : sprintf(buf, "s%03d", fCurNum); break;
			default : sprintf(buf, "l%03d", fCurNum); break;
		}
		sprintf(text, "%s%04d", buf, value);
		fLabels.insert(value_type(value, string(text)));
	}
}

RCX_Result RCX_Disasm::FindLabel(const UByte *code, int length, UShort pc)
{
	int iLength;
	const Instruction *inst;
	UByte op;
	ULong args;
	const UByte *ptr;

	// lookup the instruction
	if (length < 1) return -1;
	op = *code;
	if (ResourceType(fCurType))
		inst = fResOpDisp[op];
	else
		inst = fOpDispatch[op];
	if (!inst) return -1;

	// make sure there's enough code for entire instruction
	iLength = ArgsLength(inst->fArgs) + 1;
	if (length < iLength) return -1;

	ptr = code+1;
	for(args = inst->fArgs; args; args>>=kArgFormatWidth)
	{
		int af = args & kArgFormatMask;
		FindLabelArg(af, ptr, pc + (ptr - code));
		ptr += argFormatLengths[af];
	}

	return iLength;
}

RCX_Result RCX_Disasm::SPrint1(char *text, const UByte *code, int length, UShort pc)
{
	int iLength;
	const Instruction *inst;
	char argText[256];
	UByte op;
	ULong args;
	bool argPrinted = false;
	const UByte *ptr;

	// lookup the instruction
	if (length < 1) return -1;
	op = *code;
        if (ResourceType(fCurType))
          inst = fResOpDisp[op];
        else
	  inst = fOpDispatch[op];
	if (!inst) return -1;

	// make sure there's enough code for entire instruction
	iLength = ArgsLength(inst->fArgs) + 1;
	if (length < iLength) return -1;

	// print the instruction name
        if (fGenLASM)
            sprintf(text,"%s\t", inst->fName);
        else
            sprintf(text,"%-8s ", inst->fName);
	text += strlen(text);

	// print the args
	ptr = code+1;
	for(args = inst->fArgs; args; args>>=kArgFormatWidth)
	{
		int af = args & kArgFormatMask;

		SPrintArg(argText, af, ptr, pc + (ptr - code));
		if (argText[0])
		{
			sprintf(text, "%s%s", argPrinted ? ", " : "", argText);
			text += strlen(text);
			argPrinted = true;
		}

		ptr += argFormatLengths[af];
	}

	return iLength;
}


void RCX_Disasm::DefineInstructions(const Instruction** OpArray, const Instruction *inst)
{
	while(inst->fName)
	{
		int op = inst->fOpcode;

		#ifdef DEBUG
		if (OpArray[op] != 0)
			printf("opcode 0x%02x already defined\n", op);
		#endif

                if (fTarget != kRCX_SwanTarget)
		  OpArray[op ^ 0x8] = inst; // swan breaks this rule
		OpArray[op] = inst;


		#ifdef CHECK_LENGTHS
                // check length
                int argLen = op & 7;
                if (argLen > 5) argLen -= 6;
                if (fTarget == kRCX_SwanTarget)
                {
                  // swan breaks this rule
                  //   (you can't figure out the argument length from the opcode)
                  argLen = getSwanArgLength(op);
                }

                int computedLen = ArgsLength(inst->fArgs);

                if (computedLen != argLen)
                        printf("opcode 0x%02x: possible wrong length of %d, should be %d\n", op, computedLen, argLen);
		#endif

		++inst;
	}
}


void SPrintOutputNames(char *argText, const UByte outs)
{
	char *ptr = argText;

	if (outs)
	{
		// list outputs
		for(int i=0; i<3; i++)
			if (outs & (1 << i))
				*ptr++ = (char)('A' + i);
	}
	else
		*ptr++ = '0';

	*ptr = 0;
}


int ArgsLength(ULong args)
{
	int length = 0;

	for( ; args; args >>= kArgFormatWidth)
	{
		length += argFormatLengths[args & kArgFormatMask];
	}

	return length;
}


void RCX_Disasm::SPrintValue(char *text, int type, short data)
{
        if (fGenLASM)
        {
          sprintf(text, "%d, %d", type, data);
        }
        else
        {
            switch(type)
            {
                    case kRCX_VariableType:
                            sprintf(text, "var[%d]", data);
                            break;
                    case kRCX_ConstantType:
                            sprintf(text,"%d", data);
                            break;
                    case kRCX_IndirectType:
                            sprintf(text,"*var[%d]", data);
                            break;
                    case kRCX_UARTSetupType:
                            sprintf(text, "uart[%d]", data);
                            break;
                    case kRCX_GlobalVariableType:
                            if (fTarget == kRCX_SwanTarget)
                              sprintf(text, "gvar[%d]", data);
                            else
                              sprintf(text, "%s(%d)", GetTypeName(type), data);
                            break;
                    case kRCX_IndirectGlobalVarType:
                            if (fTarget == kRCX_SwanTarget)
                              sprintf(text, "*gvar[%d]", data);
                            else
                              sprintf(text, "%s(%d)", GetTypeName(type), data);
                            break;
                    default:
                            sprintf(text, "%s(%d)", GetTypeName(type) , data);
                            break;
            }
        }
}


const char *RCX_Disasm::GetTypeName(int type)
{
	if (fTarget == kRCX_ScoutTarget) {
		switch(type) {
			case 18: return "ScoutRules";
			case 19: return "SensorParam";
			case 20: return "TimerLimit";
                        case 22: return "CounterLimit";
                        case 24: return "EventFeedback";
		}
	}
        else if (fTarget == kRCX_CMTarget) {
                switch(type) {
                        case  5: return "TachoCount";
                        case  6: return "TachoSpeed";
                        case  7: return "MotorCurrent";
                        case 16: return "CounterLimit";
                }
        }
        else if (fTarget == kRCX_SpyboticsTarget) {
                switch(type) {
                        case 18: return "Stack";
                        case 19: return "TimerControl";
                        case 20: return "EEPROM";
                        case 22: return "LED";
                        case 32: return "TaskID";
                        case 43: return "RobotNotes";
                        case 45: return "RobotDist";
                        case 46: return "RobotDir";
                        case 47: return "RobotOrient";
                        case 49: return "RobotID";
                        case 50: return "TargetInfo";
                        case 51: return "PingCtrl";
                        case 52: return "BeaconCtrl";
                        case 53: return "SoundCtrl";
                        case 54: return "IndirectEEPROM";
                }
        }

        if (fTarget == kRCX_SwanTarget)
          return LOOKUP(type, swanTypeNames);
        else
	  return LOOKUP(type, typeNames);
}


void RCX_Disasm::SPrintCondition(char *text, const UByte *code)
{
	char v1Text[16];
	char v2Text[16];

	SPrintValue(v1Text, (code[0] & 0x3f), (short)WORD(code+2));
	SPrintValue(v2Text, (code[1] & 0x3f), code[4]);
        if (fGenLASM)
          sprintf(text, "%s, %d, %s", v1Text, (code[0]>>6) & 3, v2Text);
        else
          sprintf(text, "%s %s %s", v1Text, LOOKUP((code[0]>>6) & 3, relNames), v2Text);
}


int ComputeOffset(UByte b1, UByte b2, bool lowFirst)
{
        if (lowFirst)
        {
                int x = (b1 & 0x7f) + (b2 << 7);

                if (b1 & 0x80) x = -x;

                return x;
        }
        else
        {
                int x = (b2 & 0x7f) + (b1 << 7);

                if (b2 & 0x80) x = -x;

                return x;
        }
}


void RCX_Printer::Print(const char *text)
{
	Print(text, strlen(text));
}
