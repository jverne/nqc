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

#ifndef __RCX_Cmd_h
#define __RCX_Cmd_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#define kRCX_Cmd_MaxShortLength	8


class RCX_Cmd
{
public:
			RCX_Cmd()	{ fLength = 0; };
			~RCX_Cmd();

	int		GetLength()	const 		{ return fLength; }
	UByte*		GetBody()			{ return (fLength <= kRCX_Cmd_MaxShortLength) ? fBody.fData : fBody.fPtr; }
	const UByte*	GetBody() const			{ return (fLength <= kRCX_Cmd_MaxShortLength) ? fBody.fData : fBody.fPtr; }
	int		CopyOut(UByte *dst);

	void		SetLength(int length);
	UByte&		operator[](int index)		{ return GetBody()[index]; }
	const UByte&	operator[](int index) const	{ return GetBody()[index]; }


	RCX_Cmd*	Set(const UByte *data, int length);
	RCX_Cmd*	Set(UByte d0);
	RCX_Cmd*	Set(UByte d0, UByte d1);
	RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2);
	RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2, UByte d3);
	RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4);
        RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5);
        RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5, UByte d6);
        RCX_Cmd*	Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5, UByte d6, UByte d7);

	// utility functions to create specific commands

	// variables
        RCX_Cmd*	MakeVar(RCX_VarCode code, UByte var, RCX_Value value);
        RCX_Cmd*	MakeVarNewSrc(RCX_VarCode code, UByte var, RCX_Value value);

	// outputs
	RCX_Cmd*	MakeOutputMode(UByte outputs, RCX_OutputMode mode)	{ return Set(kRCX_OutputModeOp, (UByte)(mode | outputs)); }
	RCX_Cmd*	MakeOutputPower(UByte outputs, RCX_Value value);
	RCX_Cmd*	MakeOutputDir(UByte outputs, RCX_OutputDir dir)		{ return Set(kRCX_OutputDirOp, (UByte)(dir | outputs)); }

	// inputs
	RCX_Cmd*	MakeInputMode(UByte input, RCX_InputMode mode)		{ return Set(kRCX_InputModeOp, input, mode); }
	RCX_Cmd*	MakeInputType(UByte input, RCX_InputType type)		{ return Set(kRCX_InputTypeOp, input, type); }

	// sound
	RCX_Cmd*	MakePlaySound(UByte sound)				{ return Set(kRCX_PlaySoundOp, (UByte)(sound & 7)); }
	RCX_Cmd*	MakePlayTone(UShort freq, UByte duration);

	// control flow
	RCX_Cmd*	MakeTest(RCX_Value v1, RCX_Relation rel, RCX_Value v2, short offset);
	RCX_Cmd*	MakeJump(short offset);
	RCX_Cmd*	MakeSetLoop(RCX_Value v)		{ return MakeValue8(kRCX_SetLoopOp, v); }
	RCX_Cmd*	MakeCheckLoop(short offset);

	// misc
	RCX_Cmd*	MakeStopTask(UByte task)		{ return Set(kRCX_StopTaskOp, task); }
	RCX_Cmd*	MakeDelay(RCX_Value value)		{ return MakeValue16(kRCX_DelayOp, value); }
        RCX_Cmd*	MakeDisplay(RCX_Value value)		{ return MakeValue16(kRCX_DisplayOp, value); }
        RCX_Cmd*	MakeSet(RCX_Value dst, RCX_Value src);
        RCX_Cmd*	MakeSetMath(RCX_Value dst, RCX_Value src, RCX_VarCode op);
        RCX_Cmd*	MakePushStackEntry(RCX_Value value)	{ return MakeValue16(kRCX_PushStackEntryOp, value); }

	// system commands
	RCX_Cmd*	MakeRead(RCX_Value value)		{ return MakeValue8(kRCX_ReadOp, value); }
	RCX_Cmd*	MakeUnlock();
	RCX_Cmd*	MakeBegin(RCX_ChunkType type, UByte taskNumber, UShort length);
	RCX_Cmd*	MakeDownload(UShort seq, const UByte *data, UShort length);
	RCX_Cmd*	MakeDeleteTasks()	{ return Set(kRCX_DeleteTasksOp); }
	RCX_Cmd*	MakeDeleteSubs()	{ return Set(kRCX_DeleteSubsOp); }
	RCX_Cmd*	MakePing()			{ return Set(kRCX_PingOp); }
	RCX_Cmd*	MakeUploadDatalog(UShort start, UShort count);
	RCX_Cmd*	MakeBoot();

	// special command to unlock CyberMaster
	RCX_Cmd*	MakeUnlockCM();

	void		SetOffset(int offset);
	void		Print();

private:
	RCX_Cmd*	MakeValue16(UByte opcode, RCX_Value value);
	RCX_Cmd*	MakeValue8(UByte opcode, RCX_Value value);

	int		fLength;
	union {
		UByte	fData[kRCX_Cmd_MaxShortLength];
		UByte*	fPtr;
	} fBody;
};


#endif
