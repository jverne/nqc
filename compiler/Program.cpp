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

#include "Program.h"
#include "Fragment.h"
#include "RCX_Image.h"
#include "Bytecode.h"
#include "parser.h"
#include "Symbol.h"
#include "BlockStmt.h"
#include "CallStmt.h"
#include "FunctionDef.h"
#include "Error.h"
#include "RCX_Target.h"
#include "VarTranslator.h"
#include "DeclareStmt.h"
#include "PDebug.h"
#include "Resource.h"

Program *gProgram;



Program::Program(const RCX_Target *target) :
	fVarAllocator(target->fMaxGlobalVars, target->fMaxTaskVars)
{
	fTarget = target;

	// set fFragNumbers array to the first available number for each type
	for(int i=0; i<kRCX_ChunkTypeCount; ++i)
	{
		fChunkNumbers[i] = target->fRanges[i].fBase;
	}
	fChunkNumbers[kRCX_TaskChunk]++; // leave room for main

	fMainAdded = false;
	fScopes.InsertHead(new Scope());
	fInitName = Symbol::Get("_init");
	fInitLocation.fIndex = kIllegalSrcIndex;
	fGlobalDecls = new BlockStmt();
	fVirtualVarCount = 0;
}


Program::~Program()
{
	while(Fragment *f=fTasks.RemoveHead())
		delete f;

	while(Fragment *f=fSubs.RemoveHead())
		delete f;

	while(Scope *s=fScopes.RemoveHead())
		delete s;

	while(FunctionDef *func = fFunctions.RemoveHead())
		delete func;

	delete fGlobalDecls;
}


int Program::AddFragment(Fragment *f)
{
	CheckName(f->GetName());

	if (f->IsTask()) {
		if (f->GetName() == Symbol::Get("main"))
		{
			// main task is added to head of list and uses the base numer
			fMainAdded = true;
			fTasks.InsertHead(f);
			return fTarget->fRanges[kRCX_TaskChunk].fBase;
		}
		else
		{
			fTasks.InsertTail(f);
		}
	}
	else {
		fSubs.InsertTail(f);
	}

	int type = f->GetChunkType();
	return fChunkNumbers[type]++;
}



void Program::AddFunction(FunctionDef *f)
{
	CheckName(f->GetName());
	fFunctions.InsertTail(f);
}


void Program::AddGlobalDecls(BlockStmt *b)
{
	// 'b' should be a block statement containing
	// one or more DeclareStmt instances.  Simply move
	// all of the children of b to the fGlobalDecls
	// block.  Later on (in AllocateGlobals() we'll check
	// to make sure they are really DeclareStmts

	while(Stmt *s = b->RemoveHead())
	{
		fGlobalDecls->Add(s);
	}

	delete b;
}


void Program::AddResource(Resource *r)
{
	CheckName(r->GetName());

	RCX_ChunkType type = r->GetType();

	if (type < kRCX_SoundChunk || type >= kRCX_ChunkTypeCount)
	{
		Error(kErr_BadResourceType).RaiseLex();
		return;
	}

	if (fChunkNumbers[type] >= fTarget->GetChunkLimit(type))
	{
		Error(kErr_TooManyResources, fTarget->fRanges[type].fCount).RaiseLex();
		return;
	}

	r->SetNumber(fChunkNumbers[type]++);
	fResources.InsertTail(r);
}


void Program::CheckName(const Symbol *name)
{
	if (Defined(name))
		Error(kErr_SymRedef, name->GetKey()).RaiseLex();
}


int Program::CreateVar(const Symbol *name, bool array, bool ptr, bool stack)
{
	int var;

	var = NextVirtualVar();
	if (fScopes.GetHead()->GetNext()==0)
	{
		// global var
		// probably should remember the name somewhere
	}

	DefineVar(name, var, array, ptr, stack);
	return var;
}


int Program::NextVirtualVar()
{
	return kVirtualVarBase + fVirtualVarCount++;
}


void Program::DefineVar(const Symbol *name, int var, bool array, bool ptr, bool stack)
{
	Scope *s = fScopes.GetHead();

	if (s->Contains(name))
	{
		Error(kErr_SymRedef, name->GetKey()).RaiseLex();
	}
	else
		s->Define(name, var, array, ptr, stack);
}


int Program::GetVar(const Symbol *name, bool &array, bool &ptr, bool &stack)
{
	int var = fScopes.GetHead()->Lookup(name, array, ptr, stack);
	if (var == kIllegalVar)
		Error(kErr_Undeclared, name->GetKey());

	return var;
}


void Program::SetInitName(Symbol *name, LexLocation *location)
{
	fInitName = name;
	if (location)
	{
		fInitLocation = *location;
	}
	else
	{
		fInitLocation.fIndex = kIllegalSrcIndex;
	}
}

#include "RCX_SpyboticsLinker.h"

RCX_Image*	Program::CreateImage()
{
	RCX_Image *image;

	image = new RCX_Image();
	image->SetTargetType(fTarget->fType);

	if (!AllocateGlobals(image)) return image;
	if (!PrepareMainTask()) return image;
	if (!CheckFragments()) return image;

	// emit subs
	for(Fragment *sub=fSubs.GetHead(); sub; sub=sub->GetNext())
		EncodeFragment(image, sub);

	// emit tasks
	for(Fragment *task=fTasks.GetHead(); task; task=task->GetNext())
		EncodeFragment(image, task);

	// copy resources
	for(Resource *r=fResources.GetHead(); r; r=r->GetNext())
	{
		image->AddChunk(r->GetType(), r->GetNumber(),
			r->GetData(), r->GetLength(), r->GetName()->GetKey(),
			0, 0);
	}

	return image;
}


void Program::EncodeFragment(RCX_Image *image, Fragment *f)
{
	Bytecode *b = new Bytecode(fVarAllocator, fTarget, image);

	// determine variable allocation mode
	int mode;
	if (f->IsTask())
		mode = VarAllocator::kTaskMode;
	else if (f->GetTaskID() == Fragment::kMultiTaskID)
		mode = VarAllocator::kMultiSubMode;
	else
		mode = VarAllocator::kSingleSubMode;

	fVarAllocator.Begin(mode);

	f->Emit(*b);
	image->AddChunk(f->GetChunkType(), f->GetNumber(),
		b->GetData(), b->GetLength(), f->GetName()->GetKey(),
		b->GetSourceTags(), b->GetSourceTagCount());

	f->SetLocalMask(fVarAllocator.End());

	delete b;
}


bool Program::PrepareMainTask()
{
	// find main
	if (!fMainAdded)
	{
		Error(kErr_UndefinedMain).Raise(0);
		return false;
	}

	// insert block into main task so we have a place for init code
	Fragment *mainTask = fTasks.GetHead();
	BlockStmt *mainBlock = new BlockStmt();

	mainBlock->Add(mainTask->GetBody());
	mainTask->SetBody(mainBlock);

	// insert call for init routine into main task
	if (fInitName)
	{
		FunctionDef *f = gProgram->GetFunction(fInitName);
		if (f)
		{
			CallStmt *call = new CallStmt();
			call->SetName(fInitName);
			call->SetLocation(fInitLocation);
			mainBlock->Prepend(call);
		}
		else
		{
			Error(kErr_UnknownInit, fInitName->GetKey()).Raise(0);
			return false;
		}
	}

	// insert init stmts into main task
	mainBlock->Prepend(fGlobalDecls);
	fGlobalDecls = 0;

	return true;
}


bool Program::CheckFragments()
{
	bool ok = true;

	if (fChunkNumbers[kRCX_TaskChunk] > fTarget->GetChunkLimit(kRCX_TaskChunk))
	{
		Error(kErr_TooManyTasks, fTarget->fRanges[kRCX_TaskChunk].fCount).Raise(0);
		ok = false;
	}

	if (fChunkNumbers[kRCX_SubChunk] > fTarget->GetChunkLimit(kRCX_SubChunk))
	{
		Error(kErr_TooManySubs, fTarget->fRanges[kRCX_SubChunk].fCount).Raise(0);
		ok = false;
	}

	Fragment *f;
	for(f=fTasks.GetHead(); f; f=f->GetNext())
	{
		f->Check();
	}

	for(f=fSubs.GetHead(); f; f=f->GetNext())
	{
		f->Check();
	}

	return ok;
}


bool Program::Defined(const Symbol *name) const
{
	if (Find(fTasks, name)) return true;
	if (Find(fSubs, name)) return true;
	if (Find(fFunctions, name)) return true;
	if (Find(fResources, name)) return true;

	return fScopes.GetTail()->Contains(name);
}


Scope* Program::PushScope()
{
	Scope *s = new Scope();
	fScopes.InsertHead(s);
	return s;
}


void Program::PopScope()
{
	delete fScopes.RemoveHead();
}


bool Program::AllocateGlobals(RCX_Image *image)
{
	for(Stmt *s=fGlobalDecls->GetHead(); s; s=s->GetNext())
	{
		DeclareStmt *dec = dynamic_cast<DeclareStmt*>(s);
		PASSERT(dec);

		int from = dec->GetVar();

		// global scope
		int to = fVarAllocator.Allocate(false, false, dec->GetCount());
		if (to == kIllegalVar)
		{
			Error(kErr_NoMoreVars).Raise(&dec->GetLoc());
			return false;
		}

		image->SetVariable(to, dec->GetName()->GetKey());
		TranslateVar(from, to);
	}

	return true;
}


void Program::TranslateVar(int from, int to)
{
	VarTranslator vt(from, to);

	for(Fragment *f=fTasks.GetHead(); f; f=f->GetNext())
		Apply(f->GetBody(), vt);

	for(Fragment *f=fSubs.GetHead(); f; f=f->GetNext())
		Apply(f->GetBody(), vt);

	for(FunctionDef *f=fFunctions.GetHead(); f; f=f->GetNext())
		Apply(f->GetBody(), vt);

	Apply(fGlobalDecls, vt);
}



bool Program::ReserveVars(int start, int end)
{
	if (end < start) return false;

	for(int v=start; v<=end; ++v)
		if (!fVarAllocator.Reserve(v)) return false;

	return true;
}
