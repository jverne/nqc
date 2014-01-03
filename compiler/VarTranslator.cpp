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
#include "VarTranslator.h"
#include "Expr.h"
#include "Stmt.h"
#include "AssignStmt.h"
#include "AtomExpr.h"
#include "IncDecExpr.h"


bool VarTranslator::operator()(Stmt *s)
{
	vector<Expr*> v;
	s->GetExprs(v);

	int n = v.size();
	int i;

	for(i=0; i<n; ++i)
	{
		::Apply(v[i], *this);
	}


	if (Translatable *t = dynamic_cast<Translatable*>(s))
		t->Translate(fFrom, fTo);

	return true;
}


bool VarTranslator::operator()(Expr *e)
{
	if (Translatable *t = dynamic_cast<Translatable*>(e))
		t->Translate(fFrom, fTo);

	return true;
}



