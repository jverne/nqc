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

#ifndef __Program_h
#define __Program_h

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif

#ifndef __Scope_h
#include "Scope.h"
#endif

#ifndef __Variable_h
#include "Variable.h"
#endif

#ifndef __VarAllocator_h
#include "VarAllocator.h"
#endif

#ifndef __LexLocation_h
#include "LexLocation.h"
#endif

#ifndef __Fragment_h
#include "Fragment.h"
#endif

#ifndef __FunctionDef_h
#include "FunctionDef.h"
#endif

#ifndef __Resource_h
#include "Resource.h"
#endif



#include <vector>

using std::vector;

class Fragment;
class Resource;
class FunctionDef;
class Symbol;
class RCX_Image;
class Stmt;
class BlockStmt;

// useful templates to search through a list of named items

template <class T> T*Find(PListS<T> &list, const Symbol *name)
{
	for(T *t = list.GetHead(); t; t=t->GetNext())
	{
		if (t->GetName() == name) return t;
	}
	return 0;
}

template <class T> const T*Find(const PListS<T> &list, const Symbol *name)
{
	for(T *t = list.GetHead(); t; t=t->GetNext())
	{
		if (t->GetName() == name) return t;
	}
	return 0;
}




class Program
{
public:
	Program(const RCX_Target *target);
	~Program();


	const RCX_Target*	GetTarget() const	{ return fTarget; }

	// adding tasks and other stuff...
	int	AddFragment(Fragment *f);
	void	AddResource(Resource *r);
	void	AddFunction(FunctionDef *f);
	void	AddGlobalDecls(BlockStmt *s);

	RCX_Image	*CreateImage();

	// variable allocation, etc.
	int	CreateVar(const Symbol *name, bool array, bool ptr, bool stack);
	int	GetVar(const Symbol *name, bool &array, bool &ptr, bool &stack);
	int	NextVirtualVar();
	void	DefineVar(const Symbol *name, int var, bool array, bool ptr, bool stack);

	// getting tasks, etc
	Fragment*	GetTask(const Symbol *name) { return Find(fTasks, name); }
	Fragment*	GetSub(const Symbol *name) { return Find(fSubs, name); }
	FunctionDef*	GetFunction(const Symbol *name) { return Find(fFunctions, name); }
	const Resource*	GetResource(const Symbol *name) const { return Find(fResources, name); }

	// check all symbols (tasks, variables, etc)
	bool		Defined(const Symbol *name) const;

	Scope*		PushScope();
	void		PopScope();

	void		SetInitName(Symbol *name, LexLocation *location);

	bool		ReserveVars(int start, int end);

private:
	void		EncodeFragment(RCX_Image *image, Fragment *f);
	void		CheckName(const Symbol *name);
	bool		AllocateGlobals(RCX_Image *image);
	bool		SetMainTask();
	bool		PrepareMainTask();
	bool		CheckFragments();

	void		TranslateVar(int from, int to);

	// fields
	VarAllocator	        fVarAllocator;
	PListS<Fragment>	fTasks;
	PListS<Fragment>	fSubs;
	PListS<FunctionDef>	fFunctions;
	PListS<Resource>	fResources;

	bool		fMainAdded;
	int		fChunkNumbers[kRCX_ChunkTypeCount];

	PListS<Scope>	fScopes;
	Symbol*		fInitName;
	LexLocation	fInitLocation;

	BlockStmt*		fGlobalDecls;
	const RCX_Target*	fTarget;

	int		fVirtualVarCount;
};


extern Program *gProgram;

#endif
