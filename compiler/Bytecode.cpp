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

#include <cstring>
#include "Bytecode.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"

using std::memcpy;
using std::memset;

Bytecode::Bytecode(VarAllocator &varAllocator, const RCX_Target *target, RCX_Image *image) :
	fVarAllocator(varAllocator),
	fTarget(target),
	fImage(image)
{
	fData.reserve(256);
	fLabels.reserve(50);

	for(int i=0; i<kFlowCount; ++i)
		fFlowContexts[i].reserve(10);

	PushFlow(kContinueFlow, false);
	PushFlow(kBreakFlow, false);
	PushFlow(kReturnFlow, false);
}


Bytecode::~Bytecode()
{
	Fixup *f;

	while((f=fFixups.RemoveHead()) != nil)
		delete f;
}


void Bytecode::Add(const UByte *data, int count)
{
	int n = fData.size();

	fData.resize(n + count);
	memcpy(&fData[n], data, (size_t) count);
}


void Bytecode::Add(const RCX_Cmd &cmd)
{
	Add(cmd.GetBody(), cmd.GetLength());
}



void Bytecode::Truncate(int length)
{
	if ((int)fData.size() <= length) return;

	// shrink the data
	fData.resize(length);

	// disable any fixups in the truncated region
	Fixup *f;
	for(f=fFixups.GetHead(); f; f=f->GetNext())
	{
		if (f->fLocation >= length)
		{
			f->fType = kNoFixup;
		}
	}
}



void Bytecode::AddJump(int label)
{
	RCX_Cmd cmd;

	cmd.MakeJump(0);
	Add(cmd);
	AddFixup(kSignBitLongFixup, label, 1, kRCX_SJumpOp);
}


bool Bytecode::AddFlowJump(FlowCode code)
{
	int label = GetFlowLabel(code);
	if (label == kIllegalLabel) return false;

	for(int i=0; i<kHandlerCount; ++i)
	{
		HandlerContext &h = fHandlerContexts[i];
		if (h.fInUse &&
			GetFlowLevel(code) == h.fFlowLevels[code])
		{
			AddHandlerExit(i);
		}
	}

	AddJump(label);

	return true;
}


void Bytecode::AddHandlerExit(int i)
{
	RCX_Cmd cmd;

	if (i==kEventHandler)
		cmd.Set(kRCX_ExitEventCheckOp);
	else
		cmd.Set(kRCX_ExitAccessCtrlOp);
	Add(cmd);
}


void Bytecode::AddTest(RCX_Value v1, RCX_Relation rel, RCX_Value v2, int label)
{
	RCX_Cmd cmd;

	cmd.MakeTest(v1, rel, v2, 0);
	Add(cmd);
	AddFixup(kSimpleLongFixup, label, 6, kRCX_STestOp);
}


void Bytecode::AddMove(int dst, RCX_Value ea)
{
	if (ea == RCX_VALUE(kRCX_VariableType, dst)) return;

	RCX_Cmd cmd;
	cmd.MakeVar(kRCX_SetVar, (UByte)dst, ea);
	Add(cmd);
}


int Bytecode::NewLabel()
{
	int label = fLabels.size();

	fLabels.push_back(GetLength());

	return label;
}


void Bytecode::SetLabel(int label, int target)
{
	// don't do anything if the label is illegal
	if (label == kIllegalLabel) return;

	if (target == kCurrentPosition)
		fLabels[label] = GetLength();
	else
		fLabels[label] = (UShort)target;
}


void Bytecode::AddFixup(FixupCode type, int label, int opcodeOffset, UByte shortOpcode)
{
	Fixup *f;
	int size;

	// determine size of the fixup
	switch(type) {
		case kSimpleLongFixup:
		case kSignBitLongFixup:
			size = 2;
			break;
		case kSimpleShortFixup:
		case kSignBitShortFixup:
			size = 1;
			break;
		default:
			size = 0;
			break;
	}

	f = new Fixup();

	f->fType = type;
	f->fLocation = GetLength()-size;
	f->fLabel = label;
	f->fOpcodeOffset = opcodeOffset;
	f->fShortOpcode = shortOpcode;

	fFixups.InsertHead(f);
}


void Bytecode::OptimizeFixups()
{
	Fixup *f;
	int codeLength = GetLength();

	// the remap array initially is marked with bytes that
	// are to be deleted, then later it becomes the map
	// from old index to new one
	int *remap = new int[codeLength+1];
	memset(remap, 0, (codeLength+1) * sizeof(int));

	for(f=fFixups.GetHead(); f; f=f->GetNext()) {
		if (ShortenFixup(*f)) {
			// mark byte to be deleted
			remap[f->fLocation+1] = 1;

			// change the opcode
			fData[f->fLocation - f->fOpcodeOffset] = f->fShortOpcode;
		}
	}

	// trim the code and compute the remap array
	int offset = 0;
	for(int i=0; i<codeLength; ++i) {
		if (!remap[i]) {
			fData[offset] = fData[i];
			remap[i] = offset++;
		}
		else {
			remap[i] = offset;
		}
	}
	fData.resize(offset);

	// add remap entry for end-of-program
	remap[codeLength] = offset;

	// adjust the labels
	int i;
	for(i=0; i<(int)fLabels.size(); ++i) {
		fLabels[i] = remap[fLabels[i]];
	}

	// adjust the fixups
	for(f=fFixups.GetHead(); f; f=f->GetNext()) {
		f->fLocation = remap[f->fLocation];
	}

	// adjust source tags
	for(i=0; i<(int)fTags.size(); ++i) {
		fTags[i].fAddress = remap[fTags[i].fAddress];
	}

	delete [] remap;
}


bool Bytecode::ShortenFixup(Fixup &f)
{
	// cannot shorten if short opcode is 0
	if (f.fShortOpcode == 0) return false;

	int offset = fLabels[f.fLabel] - f.fLocation;
	switch(f.fType) {
		case kSimpleLongFixup:
			if (offset >= 0 && offset <= 255) {
				f.fType = kSimpleShortFixup;
				return true;
			}
			break;
		case kSignBitLongFixup:
			if (offset >= -127 && offset <= 127) {
				f.fType = kSignBitShortFixup;
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

void Bytecode::ApplyFixups()
{
	OptimizeFixups();

	Fixup *f;

	while((f = fFixups.RemoveHead()) != nil)
	{
		int offset = fLabels[f->fLabel] - f->fLocation;

		switch(f->fType)
		{
			case kNoFixup:
				break;
			case kSimpleLongFixup:
			case kSimpleShortFixup:
				fData[f->fLocation] = (UByte)(offset & 0xff);
				if (f->fType == kSimpleLongFixup) {
					fData[f->fLocation+1] = (UByte)((offset >> 8) & 0xff);
				}
				break;
			case kSignBitLongFixup:
			case kSignBitShortFixup:
				UByte neg = 0;

				if (offset < 0)
				{
					neg = 0x80;
					offset = -offset;
				}

				fData[f->fLocation] = (UByte)(neg | (offset & 0x7f));
				if (f->fType == kSignBitLongFixup) {
					fData[f->fLocation+1] = (UByte)((offset >> 7) & 0xff);
				}
				break;
		}

		delete f;
	}
}


int Bytecode::PushFlow(FlowCode code, bool legal)
{
	int label = legal ? NewLabel() : kIllegalLabel;
	fFlowContexts[code].push_back(label);
	return label;
}


int Bytecode::GetFlowLabel(FlowCode code)
{
	vector<int> &v = fFlowContexts[code];
	return v[v.size() - 1];
}



/*
void Bytecode::PushLoopContext(bool continueAllowed)
{
	fLoopContexts.push_back(LoopContext(
		continueAllowed ? NewLabel() : kIllegalLabel,
		NewLabel()));
}


void Bytecode::PopLoopContext(int continueTarget, int breakTarget)
{
	SetLabel(GetLoopLabel(kContinueLabel), continueTarget);
	SetLabel(GetLoopLabel(kBreakLabel), breakTarget);

	fLoopContexts.pop_back();
}


int Bytecode::GetLoopLabel(int type) const
{
	return fLoopContexts[GetLoopLevel() - 1].fLabels[type];
}


void Bytecode::PushReturnLabel()
{
	fReturnLabels.push_back(NewLabel());
	fLoopContexts.push_back(LoopContext(kIllegalLabel, kIllegalLabel));
}


void Bytecode::PopReturnLabel()
{
	fReturnLabels.pop_back();
	fLoopContexts.pop_back();
}
*/


bool Bytecode::BeginHandler(int type)
{
	HandlerContext &h = fHandlerContexts[type];

	if (h.fInUse) return false;

	for(int i=0; i<kFlowCount; ++i)
	{
		h.fFlowLevels[i] = GetFlowLevel(static_cast<FlowCode>(i));
	}
	h.fInUse = true;

	return true;
}

void Bytecode::EndHandler(int type)
{
	fHandlerContexts[type].fInUse = false;
}


int Bytecode::GetTempVar(bool canUseLocals)
{
	return fVarAllocator.Allocate(true, canUseLocals, 1);
}


void Bytecode::ReleaseTempEA(RCX_Value ea)
{
	if (RCX_VALUE_TYPE(ea) == kRCX_VariableType ||
		ea & kRCX_ValueUsesTemp)
	{
		fVarAllocator.ReleaseTemp(RCX_VALUE_DATA(ea));
	}
}


bool Bytecode::IsTempEA(RCX_Value ea) const
{
	if (RCX_VALUE_TYPE(ea) != kRCX_VariableType) return false;
	int v = RCX_VALUE_DATA(ea);
	return fVarAllocator.IsTemp(v);
}


bool Bytecode::IsLocalEA(RCX_Value ea) const
{
	if (RCX_VALUE_TYPE(ea) != kRCX_VariableType) return false;
	return RCX_VALUE_DATA(ea) >= fTarget->fMaxGlobalVars;
}


void Bytecode::AddSourceTag(int type, const LexLocation &loc)
{
	if (loc.fIndex == kIllegalSrcIndex) return;

	int n = fTags.size();
	fTags.resize(n+1);

	RCX_SourceTag &t = fTags[n];
	t.fType = type;
	t.fSrcIndex = loc.fIndex;
	t.fSrcOffset = loc.fOffset + loc.fLength-1;
	t.fAddress = GetLength();
}
