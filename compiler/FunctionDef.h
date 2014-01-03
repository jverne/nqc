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
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */


#ifndef __FunctionDef_h
#define __FunctionDef_h

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

#ifndef __PListS_h
#include "PListS.h"
#endif


#ifndef __LocationNode_h
#include "LocationNode.h"
#endif

#include <vector>

using std::vector;

class Symbol;
class Stmt;
class Scope;


class FunctionDef : public PLinkS<FunctionDef>, public AutoFree
{
public:
	enum ArgType
	{
		kConstantArg = 0,
		kIntegerArg,
		kReferenceArg,
		kConstRefArg,
                kSensorArg,
                kPointerArg,
                kConstPtrArg
	};

			FunctionDef();
			~FunctionDef();

	bool		AddArg(const Symbol *name, ArgType type);
	int		GetArgCount() const	{ return fArgs.size(); }
	const Symbol*	GetArgName(int i) const	{ return fArgs[i].fName; }
	ArgType		GetArgType(int i) const	{ return fArgs[i].fType; }
	int		GetArgVar(int i) const	{ return fArgs[i].fVar; }

	void		SetBody(Stmt *stmt);
	Stmt*		GetBody()	{ return fBody; }

	void		SetName(const Symbol *name)	{ fName = name; }
	const Symbol*	GetName() const			{ return fName; }

	// support for source code listings
	void		SetListingEnabled(bool b)	{ fListingEnabled = b; }
	bool		GetListingEnabled() const	{ return fListingEnabled; }
	void		SetLocations(LocationNode *start, LocationNode *end);
	const LexLocation& GetStartLoc() const	{ return fStart; }
	const LexLocation& GetEndLoc() const	{ return fEnd; }

	void		CreateArgVars();

	bool		IsExpanded() const	{ return fExpanded; }
	void		SetExpanded(bool b)	{ fExpanded = b; }

private:
	const Symbol*	fName;

	struct Arg
	{
		const Symbol *	fName;
		ArgType		fType;
		int		fVar;
	};

	vector<Arg>		fArgs;
	Stmt*			fBody;

	bool			fExpanded;

	bool			fListingEnabled;
	LexLocation		fStart;
	LexLocation		fEnd;
};

#endif
