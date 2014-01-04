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
