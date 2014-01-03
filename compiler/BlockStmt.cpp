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

#include "BlockStmt.h"

BlockStmt::BlockStmt()
{
}

BlockStmt::~BlockStmt()
{
	Stmt *s;

	while((s = fList.RemoveHead()) != nil)
		delete s;
}


Stmt* BlockStmt::GetChildren()
{
	return fList.GetHead();
}



void BlockStmt::Add(Stmt *s)
{
	if (s)
	{
		fList.InsertTail(s);
		Adopt(s);
	}
}


void BlockStmt::Prepend(Stmt *s)
{
	if (s)
	{
		fList.InsertHead(s);
		Adopt(s);
	}
}


void BlockStmt::EmitActual(Bytecode &b)
{
	for(Stmt *s = fList.GetHead(); s; s=s->GetNext())
	{
		s->Emit(b);
	}
}


Stmt* BlockStmt::CloneActual(Mapping *b) const
{
	BlockStmt *block = new BlockStmt;

	for(Stmt *s = fList.GetHead(); s; s=s->GetNext())
	{
		Stmt *child = s->Clone(b);
		block->fList.InsertTail(child);
		block->Adopt(child);
	}

	return block;
}
