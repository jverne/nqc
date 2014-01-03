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

#ifndef __Resource_h
#define __Resource_h

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

#include <vector>

using std::vector;

class Symbol;
class Expr;

class Resource : public PLinkS<Resource>, public AutoFree
{
public:
				Resource();
				~Resource();

	void			SetInfo(RCX_ChunkType type, Symbol *name);

	void			SetNumber(int n)	{ fNumber = n; }
	int				GetNumber()	const	{ return fNumber; }
	Symbol*			GetName() const		{ return fName; }
	RCX_ChunkType	GetType() const		{ return fType; }

	void			Add(int value)		{ fData.push_back((UByte)value); }
	int				GetLength() const	{ return fData.size(); }
	const UByte*	GetData() const	{ return &fData[0]; }

private:
	RCX_ChunkType	fType;
	Symbol*			fName;
	int				fNumber;
	vector<UByte>	fData;
};

#endif
