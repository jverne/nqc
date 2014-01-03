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
#include "GotoStmt.h"
#include "Bytecode.h"
#include "Error.h"
#include "LabelStmt.h"
#include "Symbol.h"
#include "InlineStmt.h"

class LabelSet
{
public:
	LabelStmt*	find(const Symbol *name);
	void		add(LabelStmt *s)	{ fLabels.push_back(s); }

private:
	vector<LabelStmt*>	fLabels;

};


GotoStmt::GotoStmt(const Symbol *name, const struct LexLocation &loc)
	: fName(name), fLocation(loc), fLabel(kIllegalLabel)
{
}

void GotoStmt::EmitActual(Bytecode &b)
{
	if (fLabel == kIllegalLabel)
	{
		Error(kErr_UndefinedLabel, fName->GetKey()).Raise(&fLocation);
		return;
	}

	b.AddJump(fLabel);
}


Stmt* GotoStmt::CloneActual(Mapping *) const
{
	return new GotoStmt(fName, fLocation);
}



class LabelFinder
{
public:
				LabelFinder(LabelSet &labelSet, Bytecode &b)
					: fLabelSet(labelSet), fBytecode(b) {};
	bool		operator()(Stmt *s);

private:
	LabelSet&	fLabelSet;
	Bytecode&	fBytecode;
};

bool LabelFinder::operator()(Stmt *s)
{
	// restart resolving within an InlineStmt
	InlineStmt *is = dynamic_cast<InlineStmt*>(s);
	if (is)
	{
		GotoStmt::ResolveGotos(is->GetBody(), fBytecode);
		return false;
	}

	// register the label
	LabelStmt* ls = dynamic_cast<LabelStmt*>(s);
	if (ls)
	{
		if (fLabelSet.find(ls->fName))
		{
			Error(kErr_RedefinedLabel, ls->fName->GetKey()).Raise(&ls->fLocation);
		}
		else
		{
			fLabelSet.add(ls);
			ls->fLabel = fBytecode.NewLabel();
		}
	}

	return true;
}


class GotoBinder
{
public:
				GotoBinder(LabelSet &labelSet)
					: fLabelSet(labelSet) {};
	bool		operator()(Stmt *s);

private:
	LabelSet&	fLabelSet;
};


bool GotoBinder::operator()(Stmt *s)
{
	// stop at InlineStmt
	if (dynamic_cast<InlineStmt*>(s))
	{
		return false;
	}

	// resolve the label
	GotoStmt* gs = dynamic_cast<GotoStmt*>(s);
	if (gs)
	{
		LabelStmt* ls = fLabelSet.find(gs->fName);
		if (ls)
		{
			gs->fLabel = ls->GetLabel();

			// force everything from the labelstmt up to a common parent to emit
			Stmt* ancestor = ls;

			while(ancestor)
			{
				if (s->IsDescendantOf(ancestor)) break;
				ancestor->SetMustEmit(true);
				ancestor = ancestor->GetParent();
			}
		}
	}

	return true;
}


void GotoStmt::ResolveGotos(Stmt *s, Bytecode &b)
{
	LabelSet labelSet;

	LabelFinder lf(labelSet, b);
	Apply(s, lf);

	GotoBinder gb(labelSet);
	Apply(s, gb);
}


LabelStmt* LabelSet::find(const Symbol *s)
{
	for(int i=0; i<(int)fLabels.size(); ++i)
	{
		if (fLabels[i]->GetName() == s) return fLabels[i];
	}
	return 0;
}
