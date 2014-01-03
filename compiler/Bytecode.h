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

#ifndef __Bytecode_h
#define __Bytecode_h

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __JumpStmt_h
#include "JumpStmt.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __VarAllocator_h
#include "VarAllocator.h"
#endif

#ifndef __RCX_Image_h
#include "RCX_Image.h"
#endif

#ifndef __RCX_SourceTag_h
#include "RCX_SourceTag.h"
#endif

#include <vector>

using std::vector;

class RCX_Cmd;

#define kCurrentPosition	-1
#define kIllegalLabel		-1

class VarPool;
class RCX_Target;

class Bytecode
{
public:
	enum HandlerCode
	{
		// codes for Handler contexts
		kEventHandler = 0,
		kAcquireHandler,
		kHandlerCount	// not an actual handler - must be the last code
	};

	enum FlowCode
	{
		// codes for Flow contexts
		kContinueFlow = 0,
		kBreakFlow,
		kReturnFlow,
		kFlowCount	// not an actual flow - must be the last code
	};

	enum FixupCode
	{
		kNoFixup = 0, // used to invalidiate a fixup
		kSimpleLongFixup,
		kSimpleShortFixup,
		kSignBitLongFixup,
		kSignBitShortFixup
	};

	Bytecode(VarAllocator &varAllocator, const RCX_Target *target, RCX_Image *image);
	~Bytecode();

	int		GetLength()	const	{ return fData.size(); }
	const UByte*	GetData() const		{ return &fData[0]; }

	void		Add(const RCX_Cmd &cmd);
	void		AddJump(int label);
	void		AddTest(RCX_Value v1, RCX_Relation rel, RCX_Value v2, int label);
	void		AddMove(int dst, RCX_Value ea);
	void		Truncate(int length);

	bool		AddFlowJump(FlowCode code);

	int		NewLabel();
	void		SetLabel(int label, int target = kCurrentPosition);
	int		GetLabelPosition(int label) const { return fLabels[label]; }
	void		AddFixup(FixupCode code, int label, int opcodeOffset=0, UByte shortOpcode=0);
	void		ApplyFixups();

	int		PushFlow(FlowCode code, bool legal=true);
	void		PopFlow(FlowCode code)		{ fFlowContexts[code].pop_back(); }

	bool		BeginHandler(int type);
	void		EndHandler(int type);

	int		GetTempVar(bool canUseLocal);
	void		ReleaseTempEA(RCX_Value ea);
	bool		IsTempEA(RCX_Value ea) const;
	bool		IsLocalEA(RCX_Value ea) const;

	VarAllocator&	GetVarAllocator()	{ return fVarAllocator; }

	const RCX_Target*	GetTarget() const	{ return fTarget; }

	void		AddVariableName(int index, const char *name) { fImage->SetVariable(index, name); }

	// source tags
	void			AddSourceTag(int type, const LexLocation &loc);
	const RCX_SourceTag*	GetSourceTags() const		{ return &fTags[0]; }
	int			GetSourceTagCount() const	{ return fTags.size(); }

private:
	class Fixup : public PLinkS<Fixup>
	{
	public:
		FixupCode 	fType;
		UShort		fLocation;
		int		fLabel;
		int		fOpcodeOffset;
		int		fShortOpcode;
	};

	struct HandlerContext
	{
		HandlerContext() : fInUse(false)	{ }

		int	fFlowLevels[kFlowCount];
		bool	fInUse;
	};

	int		GetFlowLevel(FlowCode code)	{ return fFlowContexts[code].size(); }
	int		GetFlowLabel(FlowCode code);
	void		Add(const UByte *data, int count);
	void		AddHandlerExit(int i);
	void		OptimizeFixups();
	bool		ShortenFixup(Fixup &f);

	vector<UByte>	fData;

	PListSS<Fixup>	fFixups;
	vector<UShort>	fLabels;

	vector<int>	fFlowContexts[kFlowCount];
	HandlerContext	fHandlerContexts[kHandlerCount];

	VarAllocator&		fVarAllocator;
	const RCX_Target*	fTarget;
	RCX_Image*		fImage; // used to hold symbolic information

	// source info (used for mixed source/code listings
	vector<RCX_SourceTag>	fTags;
};

#endif
