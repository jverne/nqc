/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */
#ifndef __Fragment_h
#define __Fragment_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __LocationNode_h
#include "LocationNode.h"
#endif

#ifndef __FunctionDef_h
#include "FunctionDef.h"
#endif

class Symbol;
class Stmt;
class Bytecode;
class Program;
class VarPool;

class Fragment : public PLinkS<Fragment>, public AutoFree
{
public:
	enum
	{
		// special task IDs
		kNoTaskID = -1,
		kMultiTaskID = -2
	};

        Fragment(bool isTask, Symbol *name, Stmt *body);
        Fragment(bool isTask);
	~Fragment();

        bool		AddArg(const Symbol *name, FunctionDef::ArgType type);
        int		GetArgCount() const	{ return fArgs.size(); }
        const Symbol*	GetArgName(int i) const	{ return fArgs[i].fName; }
        FunctionDef::ArgType		GetArgType(int i) const	{ return fArgs[i].fType; }
        int		GetArgVar(int i) const	{ return fArgs[i].fVar; }

	void		SetLocations(LocationNode *start, LocationNode *end);

        void		CreateArgVars();

	int		GetNumber()	const	{ return fNumber; }
        void		SetName(Symbol *name);
	Symbol*		GetName() const		{ return fName; }
	bool		IsTask() const		{ return fIsTask; }
	RCX_ChunkType	GetChunkType() const	{ return fIsTask ? kRCX_TaskChunk : kRCX_SubChunk; }
	int		GetTaskID() const	{ return fTaskID; }
        void		SetBody(Stmt *stmt);
	Stmt*		GetBody()		{ return fBody; }
	int		GetLocalMask() const	{ return fLocalMask; }
	void		SetLocalMask(int m)	{ fLocalMask = m; }

	void		SetNumber(int n)	{ fNumber = n; }
	void		AssignTaskID(int n);	// only affects kRCX_SubFragment types

	void		Emit(Bytecode &b);
	void		Check();

private:
        struct Arg
        {
                const Symbol *	        fName;
                FunctionDef::ArgType    fType;
                int                     fVar;
        };

        vector<Arg>	fArgs;
	bool		fIsTask;
	Symbol*		fName;
	Stmt*		fBody;
	int		fNumber;
	int		fTaskID;
	int		fLocalMask;
	LexLocation	fStart;
	LexLocation	fEnd;
};

#endif
