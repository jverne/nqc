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
#ifndef __BlockStmt_h
#define __BlockStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif


class BlockStmt : public Stmt
{
public:
				BlockStmt();
				~BlockStmt();

	virtual Stmt*		GetChildren();


	void			Add(Stmt *s);
	void			Prepend(Stmt *s);
	const Stmt*		GetHead() const	{ return fList.GetHead(); }
	Stmt*			GetHead() 		{ return fList.GetHead(); }
	Stmt*			RemoveHead()	{ return fList.RemoveHead(); }

	virtual void	EmitActual(Bytecode &b);
	virtual Stmt*	CloneActual(Mapping *b) const;

private:
	PListS<Stmt>	fList;
};


#endif
