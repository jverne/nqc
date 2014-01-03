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



