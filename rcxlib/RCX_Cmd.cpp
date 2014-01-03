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
#include "RCX_Cmd.h"
#include "Program.h"

using std::printf;
using std::memcpy;

// handy macros
#define LO_BYTE(s)	((UByte)((s) & 0xff))
#define HI_BYTE(s)	((UByte)(((s) >> 8) & 0xff))

RCX_ValueType GetRcxValueType(int v)
{
        RCX_ValueType result = RCX_VALUE_TYPE(v);
        if (result == kRCX_VariableType || result == kRCX_IndirectType)
        {
                const RCX_Target *t = gProgram->GetTarget();
                if (t->fType == kRCX_SwanTarget)
                    result = (result == kRCX_VariableType) ?
                        kRCX_GlobalVariableType : kRCX_IndirectGlobalVarType;
        }
        return result;
}

RCX_Cmd::~RCX_Cmd()
{
	if (fLength > kRCX_Cmd_MaxShortLength)
		delete [] fBody.fPtr;
}


void RCX_Cmd::SetLength(int length)
{
	if (fLength == length) return;

	if (fLength > kRCX_Cmd_MaxShortLength)
		delete [] fBody.fPtr;

	fLength = length;

	if (fLength > kRCX_Cmd_MaxShortLength)
		fBody.fPtr = new UByte[fLength];
}


int RCX_Cmd::CopyOut(UByte *dst)
{
	memcpy(dst, GetBody(), (ULong)fLength);
	return fLength;
}


RCX_Cmd* RCX_Cmd::Set(const UByte *data, int length)
{
	SetLength(length);
	memcpy(GetBody(), data, (size_t)length);
	return this;
}


RCX_Cmd* RCX_Cmd::Set(UByte d0)
{
	SetLength(1);
	fBody.fData[0] = d0;
	return this;
}


RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1)
{
	SetLength(2);
	fBody.fData[0] = d0;
	fBody.fData[1] = d1;
	return this;
}


RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2)
{
	SetLength(3);
	fBody.fData[0] = d0;
	fBody.fData[1] = d1;
	fBody.fData[2] = d2;
	return this;
}



RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2, UByte d3)
{
	SetLength(4);
	fBody.fData[0] = d0;
	fBody.fData[1] = d1;
	fBody.fData[2] = d2;
	fBody.fData[3] = d3;
	return this;
}


RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4)
{
	SetLength(5);
	fBody.fData[0] = d0;
	fBody.fData[1] = d1;
	fBody.fData[2] = d2;
	fBody.fData[3] = d3;
	fBody.fData[4] = d4;
	return this;
}


RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5)
{
	SetLength(6);
	fBody.fData[0] = d0;
	fBody.fData[1] = d1;
	fBody.fData[2] = d2;
	fBody.fData[3] = d3;
	fBody.fData[4] = d4;
	fBody.fData[5] = d5;
	return this;
}

RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5, UByte d6)
{
        SetLength(7);
        fBody.fData[0] = d0;
        fBody.fData[1] = d1;
        fBody.fData[2] = d2;
        fBody.fData[3] = d3;
        fBody.fData[4] = d4;
        fBody.fData[5] = d5;
        fBody.fData[6] = d6;
        return this;
}

RCX_Cmd* RCX_Cmd::Set(UByte d0, UByte d1, UByte d2, UByte d3, UByte d4, UByte d5, UByte d6, UByte d7)
{
        SetLength(8);
        fBody.fData[0] = d0;
        fBody.fData[1] = d1;
        fBody.fData[2] = d2;
        fBody.fData[3] = d3;
        fBody.fData[4] = d4;
        fBody.fData[5] = d5;
        fBody.fData[6] = d6;
        fBody.fData[7] = d7;
        return this;
}


RCX_Cmd* RCX_Cmd::MakeUnlock()
{
	return Set(kRCX_UnlockOp, 1, 3, 5, 7, 0xb);
}

RCX_Cmd* RCX_Cmd::MakePlayTone(UShort freq, UByte duration)
{
	return Set(kRCX_PlayToneOp, LO_BYTE(freq), HI_BYTE(freq), duration);
}


RCX_Cmd* RCX_Cmd::MakeBegin(RCX_ChunkType type, UByte taskNumber, UShort length)
{
	UByte op = (UByte)((type == kRCX_TaskChunk) ? kRCX_BeginTaskOp : kRCX_BeginSubOp);
	return Set(op, 0, taskNumber, 0, LO_BYTE(length), HI_BYTE(length));
}


RCX_Cmd* RCX_Cmd::MakeOutputPower(UByte outputs, RCX_Value value)
{
	return Set(kRCX_OutputPowerOp, outputs, GetRcxValueType(value), (UByte)RCX_VALUE_DATA(value));
}


RCX_Cmd* RCX_Cmd::MakeValue16(UByte op, RCX_Value value)
{
	int data = RCX_VALUE_DATA(value);

	return Set(op, GetRcxValueType(value), LO_BYTE(data), HI_BYTE(data));
}

RCX_Cmd* RCX_Cmd::MakeValue8(UByte op, RCX_Value value)
{
	return Set(op, GetRcxValueType(value), (UByte)RCX_VALUE_DATA(value));
}

RCX_Cmd* RCX_Cmd::MakeSet(RCX_Value dst, RCX_Value src)
{
        UByte dType = GetRcxValueType(dst);

	SetLength(6);
	UByte *ptr = GetBody();

	*ptr++ = kRCX_SetSrcValueOp;
	*ptr++ = dType;
	*ptr++ = LO_BYTE(RCX_VALUE_DATA(dst));
	*ptr++ = GetRcxValueType(src);
	*ptr++ = LO_BYTE(RCX_VALUE_DATA(src));
	*ptr   = HI_BYTE(RCX_VALUE_DATA(src));

	return this;
}

RCX_Cmd* RCX_Cmd::MakeSetMath(RCX_Value dst, RCX_Value src, RCX_VarCode op)
{
        // this method is only for firmwares which have math source value opcodes
        // (only Swan at this point)
        SetLength(7);
        UByte *ptr = GetBody();
        RCX_ValueType vtDst = GetRcxValueType(dst);
        if (vtDst == kRCX_VariableType          ||
            vtDst == kRCX_IndirectType          ||
            vtDst == kRCX_GlobalVariableType    ||
            vtDst == kRCX_IndirectGlobalVarType ||
            vtDst == kRCX_IndexedArrayType)
        {
          switch(op)
          {
            case kRCX_AddVar: *ptr++ = rcxAddToSourceValue;         break;
            case kRCX_SubVar: *ptr++ = rcxMinusToSourceValue;       break;
            case kRCX_DivVar: *ptr++ = rcxDivideToSourceValue;      break;
            case kRCX_MulVar: *ptr++ = rcxTimesToSourceValue;       break;
            case kRCX_SgnVar: *ptr++ = rcxSignValueSourceValue;     break;
            case kRCX_AbsVar: *ptr++ = rcxAbsoluteValueSourceValue; break;
            case kRCX_AndVar: *ptr++ = rcxAndToSourceValue;         break;
            case kRCX_OrVar : *ptr++ = rcxOrToSourceValue;          break;
            case kRCX_NotVar: *ptr++ = rcxBitComplementSourceValue; break;
            case kRCX_ModVar: *ptr++ = rcxModuloToSourceValue;      break;
            case kRCX_ShlVar: *ptr++ = rcxShiftLeft;                break;
            case kRCX_ShrVar: *ptr++ = rcxShiftRight;               break;
            case kRCX_XOrVar: *ptr++ = rcxXOr;                      break;
            default :         *ptr++ = rcxAssignSourceValue;        break;
          }
        }
        else
        {
          // the destination source is not a "RAM memory variable" source so use the
          // generic source opcodes instead
          switch(op)
          {
            case kRCX_AddVar: *ptr++ = rcxAddToGenericParms;         break;
            case kRCX_SubVar: *ptr++ = rcxMinusToGenericParms;       break;
            case kRCX_DivVar: *ptr++ = rcxDivideToGenericParms;      break;
            case kRCX_MulVar: *ptr++ = rcxTimesToGenericParms;       break;
            case kRCX_SgnVar: *ptr++ = rcxSignValueSourceValue;      break;
            case kRCX_AbsVar: *ptr++ = rcxAbsoluteValueSourceValue;  break;
            case kRCX_AndVar: *ptr++ = rcxAndToGenericParms;         break;
            case kRCX_OrVar : *ptr++ = rcxOrToGenericParms;          break;
            case kRCX_NotVar: *ptr++ = rcxBitComplementGenericParms; break;
            case kRCX_ModVar: *ptr++ = rcxModuloToGenericParms;      break;
            case kRCX_ShlVar: *ptr++ = rcxShiftLeft;                 break;
            case kRCX_ShrVar: *ptr++ = rcxShiftRight;                break;
            case kRCX_XOrVar: *ptr++ = rcxXOr;                       break;
            default :         *ptr++ = rcxAssignSourceValue;         break;
          }
        }

        *ptr++ = vtDst;
        *ptr++ = LO_BYTE(RCX_VALUE_DATA(dst));
        *ptr++ = HI_BYTE(RCX_VALUE_DATA(dst));
        *ptr++ = GetRcxValueType(src);
        *ptr++ = LO_BYTE(RCX_VALUE_DATA(src));
        *ptr   = HI_BYTE(RCX_VALUE_DATA(src));

        return this;
}

RCX_Cmd* RCX_Cmd::MakeTest(RCX_Value v1, RCX_Relation rel, RCX_Value v2, short offset)
{
	int d1 = RCX_VALUE_DATA(v1);
	UByte *ptr;
	SetLength(8);

	ptr = GetBody();

	*ptr++ = kRCX_TestOp;
	*ptr++ = (UByte)((rel << 6) | GetRcxValueType(v1));
	*ptr++ = GetRcxValueType(v2);
	*ptr++ = LO_BYTE(d1);
	*ptr++ = HI_BYTE(d1);
	*ptr   = (UByte)RCX_VALUE_DATA(v2);
	SetOffset(offset);
	return this;
}


RCX_Cmd* RCX_Cmd::MakeCheckLoop(short offset)
{
	Set(kRCX_CheckLoopOp, 0, 0);
	SetOffset(offset);
	return this;
}


RCX_Cmd* RCX_Cmd::MakeJump(short offset)
{
	SetLength(3);

	GetBody()[0] = kRCX_JumpOp;
	SetOffset(offset);
	return this;
}


void RCX_Cmd::SetOffset(int offset)
{
	UByte *ptr;
	UByte neg = 0;

	offset -= fLength - 2;
	ptr = GetBody() + fLength - 2;

	if (GetBody()[0] != kRCX_JumpOp)
	{
		*ptr++ = (UByte)(offset & 0xff);
		*ptr   = (UByte)((offset >> 8) & 0xff);
	}
	else
	{
		if (offset < 0)
		{
			neg = 0x80;
			offset = -offset;
		}

		*ptr++ = (UByte)(neg | (offset & 0x7f));
		*ptr   = (UByte)((offset >> 7) & 0xff);
	}
}


RCX_Cmd* RCX_Cmd::MakeDownload(UShort seq, const UByte *data, UShort length)
{
	UByte *ptr;
	UByte checksum = 0;

	SetLength(6 + length);

	ptr = GetBody();
	*ptr++ = kRCX_DownloadOp;
	*ptr++ = LO_BYTE(seq);
	*ptr++ = HI_BYTE(seq);
	*ptr++ = LO_BYTE(length);
	*ptr++ = HI_BYTE(length);

	while(length--)
	{
		UByte b = *data++;
		checksum += b;
		*ptr++ = b;
	}

	*ptr = checksum;
	return this;
}


RCX_Cmd* RCX_Cmd::MakeUploadDatalog(UShort start, UShort count)
{
	return Set(kRCX_UploadDatalogOp,
		LO_BYTE(start), HI_BYTE(start),
		LO_BYTE(count), HI_BYTE(count));
}


RCX_Cmd* RCX_Cmd::MakeVarNewSrc(RCX_VarCode code, UByte var, RCX_Value value)
{
    int data = RCX_VALUE_DATA(value);
    RCX_ValueType dsrc = GetRcxValueType(value);
    if (code == kRCX_SgnVar || code == kRCX_AbsVar || code >= kRCX_ShlVar)
    {
        UByte x;
        switch (code)
        {
          case kRCX_SgnVar : x = rcxSignValueSourceValue;     break;
          case kRCX_AbsVar : x = rcxAbsoluteValueSourceValue; break;
          case kRCX_ShlVar : x = rcxShiftLeft;                break;
          case kRCX_ShrVar : x = rcxShiftRight;               break;
          default          : x = rcxXOr;                      break;
        }
        return Set(x, kRCX_GlobalVariableType, var, 0x00, dsrc, LO_BYTE(data), HI_BYTE(data));
    }
    else
    {
        // use global variable source and associated opcodes here
        const UByte opcodes[2][11][2] =
        {
          // non-constant operand
          { {rcxAssignGlobalVariable, rcxAssignSourceValue},
            {rcxAddToGlobal, rcxAddToSourceValue},
            {rcxMinusToGlobal, rcxMinusToSourceValue},
            {rcxDivideToGlobal, rcxDivideToSourceValue},
            {rcxTimesToGlobal, rcxTimesToSourceValue}, {0, 0}, {0, 0},
            {rcxAndToGlobal, rcxAndToSourceValue},
            {rcxOrToGlobal, rcxOrToSourceValue},
            {rcxBitComplementGlobal, rcxBitComplementSourceValue},
            {rcxModuloToGlobal, rcxModuloToSourceValue} },
          // constant operand
          { {rcxAssignGlobalConstantByte, rcxAssignGlobalConstant},
            {rcxAddToGlobalUbyte, rcxAddToGlobalConstant},
            {rcxMinusToGlobalUbyte, rcxMinusToGlobalConstant},
            {rcxDivideToGlobalUbyte, rcxDivideToGlobalConstant},
            {rcxTimesToGlobalUbyte, rcxTimesToGlobalConstant}, {0, 0}, {0, 0},
            {rcxAndToGlobalUbyte, rcxAndToGlobalConstant},
            {rcxOrToGlobalUbyte, rcxOrToGlobalConstant},
            {rcxBitComplementGlobalByte, rcxBitComplementGlobalConstant},
            {rcxModuloToGlobalUbyte, rcxModuloToGlobalConstant} }
        };
        UByte x = opcodes[dsrc == kRCX_ConstantType][code][HI_BYTE(data) != 0];
        if (dsrc == kRCX_ConstantType)
        {
            if (HI_BYTE(data) == 0)
              return Set(x, var, LO_BYTE(data));
            else
              return Set(x, var, HI_BYTE(data), LO_BYTE(data));
        }
        else
        {
            if (HI_BYTE(data) == 0)
              return Set(x, var, dsrc, LO_BYTE(data));
            else
              return Set(x, kRCX_GlobalVariableType, var, 0x00, dsrc, LO_BYTE(data), HI_BYTE(data));
        }
    }
}


RCX_Cmd* RCX_Cmd::MakeVar(RCX_VarCode code, UByte var, RCX_Value value)
{
        const RCX_Target *t = gProgram->GetTarget();
        if (t->fHasExtendedMathOps)
          return MakeVarNewSrc(code, var, value);

        int data = RCX_VALUE_DATA(value);
        return Set((UByte)kRCX_VarOp(code), var, GetRcxValueType(value), LO_BYTE(data), HI_BYTE(data));
}


RCX_Cmd* RCX_Cmd::MakeBoot()
{

	Set(kRCX_BootOp, 0x4c, 0x45, 0x47, 0x4f, 0xae);
	return this;
};


RCX_Cmd* RCX_Cmd::MakeUnlockCM()
{
	static const UByte unlockMsg[] = {kRCX_BootOp,
	  'D','o',' ','y','o','u',' ','b','y','t','e',',',' ',
	  'w','h','e','n',' ','I',' ','k','n','o','c','k','?' };
	 Set(unlockMsg, sizeof(unlockMsg));

	 return this;
}


void RCX_Cmd::Print()
{
	UByte *ptr = GetBody();

	for(int i=0; i<fLength; i++)
		printf("%02x ", ptr[i]);
	printf("\n");
}
