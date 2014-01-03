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

#include "Fragment.h"
#include "BlockStmt.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Program.h"
#include "Symbol.h"
#include "CallStmt.h"
#include "DeclareStmt.h"
#include "TaskIdExpr.h"
#include "GotoStmt.h"

#ifdef DEBUG
//#ifdef __MWERKS__
#include <typeinfo>
//#endif
// uncomment this to print the statement tree
//#define DEBUG_DUMP_FRAGMENT
#endif


Fragment::Fragment(bool isTask, Symbol *name, Stmt *body)
{
	fIsTask = isTask;
	fName = name;
	fBody = body;

	fNumber = gProgram->AddFragment(this);
	fTaskID = isTask ? fNumber : kNoTaskID;

	fStart.fIndex = kIllegalSrcIndex;
	fEnd.fIndex = kIllegalSrcIndex;
}

Fragment::Fragment(bool isTask) : fName(0), fBody(0)
{
        fIsTask = isTask;

        fStart.fIndex = kIllegalSrcIndex;
        fEnd.fIndex = kIllegalSrcIndex;
}


Fragment::~Fragment()
{
	delete fBody;
}


void Fragment::SetName(Symbol *name)
{
        fName = name;
        fNumber = gProgram->AddFragment(this);
        fTaskID = fIsTask ? fNumber : kNoTaskID;
}

void Fragment::SetBody(Stmt *s)
{
//        delete fBody; // this caused a call to a pure virtual function...
        fBody = s;
}

bool Fragment::AddArg(const Symbol *name, FunctionDef::ArgType type)
{
        Arg a;

        for(size_t i=0; i<fArgs.size(); i++)
        {
                if (fArgs[i].fName == name) return false;
        }

        a.fName = name;
        a.fType = type;

        fArgs.push_back(a);
        return true;
}

void Fragment::CreateArgVars()
{
        // if a fragment has arguments then it must be
        // a target which supports the stack so
        // get the variable off the stack
        for(size_t i=0; i<fArgs.size(); i++)
        {
                int var;

                var = gProgram->NextVirtualVar();

                switch(fArgs[i].fType)
                {
                        case FunctionDef::kConstantArg:
                                var |= kVirtualConstantFlag | kVirtualReadOnlyFlag;
                                break;
                        case FunctionDef::kConstRefArg:
                        case FunctionDef::kSensorArg:
                                var |= kVirtualReadOnlyFlag;
                                break;
                        case FunctionDef::kPointerArg:
                                var |= kPointerFlag;
                                break;
                        case FunctionDef::kConstPtrArg:
                                var |= kVirtualReadOnlyFlag | kPointerFlag;
                                break;
                        default:
                                break;
                }

                fArgs[i].fVar = var;
                gProgram->DefineVar(fArgs[i].fName, var, false,
                  ((fArgs[i].fType == FunctionDef::kPointerArg) ||
                   (fArgs[i].fType == FunctionDef::kConstPtrArg)),
                  true);
        }
}

void Fragment::SetLocations(LocationNode *start, LocationNode *end)
{
	fStart = start->GetLoc();
	delete start;

	fEnd = end->GetLoc();
	delete end;
}


void Fragment::AssignTaskID(int n)
{
	if (n == fTaskID) return;

	if (fTaskID == kNoTaskID)
		fTaskID = n;
	else
		fTaskID = kMultiTaskID;
}


void Fragment::Emit(Bytecode &b)
{
	b.AddSourceTag(RCX_SourceTag::kBegin, fStart);

	// bind TaskIdExprs to the actual task id
	// this must be done in Emit() rather than
	// Check() since subroutine trace isn't
	// complete until all fragments are checked
	TaskIdExpr::Patcher p(fTaskID);
	Apply(fBody, p);


	// resolve gotos - must be done in Emit() rather than
	// Check() since bytecode labels need to be generated
	GotoStmt::ResolveGotos(fBody, b);

	int rLabel = b.PushFlow(Bytecode::kReturnFlow);

	fBody->Emit(b);

	b.SetLabel(rLabel);
	b.PopFlow(Bytecode::kReturnFlow);

	b.ApplyFixups();

	b.AddSourceTag(RCX_SourceTag::kEnd, fEnd);
}


#ifdef DEBUG

using std::printf;

class ParentChecker
{
public:
	bool operator()(Stmt *s);
};

bool ParentChecker::operator()(Stmt *s)
{
	Stmt *c;
	for(c=s->GetChildren(); c; c=c->GetNext())
	{
		if (c->GetParent() != s)
		{
			printf("%s  %08x\n", typeid(*s).name(), s);
			printf("Parent mistmatch: Found %08x expected %08x\n", c->GetParent(), s);
		}
	}

	return true;
}
#endif




void Fragment::Check()
{
#ifdef DEBUG_DUMP_FRAGMENT
	DumpStmt(fBody);
#endif

	CallStmt::Expander e(this);
	Apply(fBody, e);

	DeclareStmt::Binder b(0);
	Apply(fBody, b);


#ifdef DEBUG
	ParentChecker p;
	Apply(fBody, p);
#endif

#ifdef DEBUG_DUMP_FRAGMENT
	DumpStmt(fBody);
#endif
}


