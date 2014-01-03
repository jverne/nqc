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

#include "MonitorStmt.h"
#include "Bytecode.h"
#include "Expr.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"
#include "Error.h"
#include "CatchStmt.h"

#define SCOUT_MONITOR_MASK (TYPEMASK(kRCX_VariableType) + \
							TYPEMASK(kRCX_ConstantType) + \
							TYPEMASK(kRCX_RandomType))


MonitorStmt::MonitorStmt(Expr *events, Stmt* body, BlockStmt *handlers, const LexLocation &loc) :
	BinaryStmt(body, handlers), fEvents(events), fLocation(loc)
{
}


MonitorStmt::~MonitorStmt()
{
	delete fEvents;
}


void MonitorStmt::GetExprs(vector<Expr*> & v) const
{
	v.push_back(fEvents);
}


void MonitorStmt::EmitActual(Bytecode &b)
{
	if (!b.GetTarget()->fEvents)
	{
		Error(kErr_NoTargetEvents, b.GetTarget()->fName).Raise(&fLocation);
		return;
	}

	if (!b.BeginHandler(Bytecode::kEventHandler))
	{
		Error(kErr_NoNestedEvents).Raise(&fLocation);
		return;
	}

	RCX_Value ea;
	RCX_Cmd cmd;
	int handlerLabel = b.NewLabel();
	int endLabel = b.NewLabel();

	// get EA of event mask
	if (b.GetTarget()->fType == kRCX_ScoutTarget)
		ea = fEvents->EmitConstrained(b, SCOUT_MONITOR_MASK);
	else
		ea = fEvents->EmitAny(b);

	if (ea==Expr::kIllegalEA) return;

	// start event monitoring
	cmd.Set(kRCX_StartEventMonOp, RCX_VALUE_TYPE(ea), RCX_VALUE_DATA(ea), RCX_VALUE_DATA(ea) >> 8, 0, 0);
	b.Add(cmd);
	b.AddFixup(Bytecode::kSignBitLongFixup, handlerLabel, 4, kRCX_SStartEventMonOp);
	b.ReleaseTempEA(ea);

	GetPrimary()->Emit(b);

	// end event monitoring
	cmd.Set(kRCX_ExitEventCheckOp);
	b.Add(cmd);

	// now iterate through handlers (which must be a block stmt
	BlockStmt *handlers = dynamic_cast<BlockStmt*>(GetSecondary());
	if (handlers)
	{
		for(Stmt *h=handlers->GetHead(); h; h=h->GetNext())
		{
			b.AddJump(endLabel);
			b.SetLabel(handlerLabel);

			// prepare next handler test
//                        handlerLabel = b.NewLabel();
                        b.NewLabel();

			CatchStmt* cs = dynamic_cast<CatchStmt*>(h);
			if (cs)
			{
				// conditional catch needs a daisy-chained label
				handlerLabel  = b.NewLabel();
				cs->SetLabel(handlerLabel);
			}
			else
				handlerLabel = kIllegalLabel;

			h->Emit(b);
		}
	}
	if (handlerLabel != kIllegalLabel)
		b.SetLabel(handlerLabel);

	b.SetLabel(endLabel);

	b.EndHandler(Bytecode::kEventHandler);
}


Stmt* MonitorStmt::CloneActual(Mapping *b) const
{
	BlockStmt *s = GetSecondary() ? (BlockStmt*)GetSecondary()->Clone(b) : 0;
	return new MonitorStmt(fEvents->Clone(b), GetPrimary()->Clone(b), s, fLocation);
}
