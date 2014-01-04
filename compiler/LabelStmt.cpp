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
#include "LabelStmt.h"
#include "Bytecode.h"
#include "Error.h"
#include "Symbol.h"


LabelStmt::LabelStmt(const Symbol *name, const LexLocation &loc, Stmt *s) :
	ChainStmt(s),
	fName(name),
	fLocation(loc)
{
}


LabelStmt::~LabelStmt()
{
}


void LabelStmt::EmitActual(Bytecode &b)
{
	b.SetLabel(fLabel);
	GetBody()->Emit(b);
}


Stmt* LabelStmt::CloneActual(Mapping *m) const
{
	return new LabelStmt(fName, fLocation, GetBody()->Clone(m));
}
