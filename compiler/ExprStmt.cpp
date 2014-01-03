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
#include <cstring>
#include "ExprStmt.h"
#include "Bytecode.h"
#include "Expr.h"


ExprStmt::ExprStmt(Expr * value)
 : fValue(value)
{
}


ExprStmt::~ExprStmt()
{
	delete fValue;
}


void ExprStmt::EmitActual(Bytecode &b)
{
	fValue->EmitSide(b);
}


Stmt* ExprStmt::CloneActual(Mapping *b) const
{
	return new ExprStmt(fValue->Clone(b));
}


void ExprStmt::GetExprs(vector<Expr *> &v) const
{
	v.push_back(fValue);
}


