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
