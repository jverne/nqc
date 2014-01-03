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
#ifndef __VarTranslator_h
#define __VarTranslator_h

class Stmt;
class Expr;

class VarTranslator
{
public:
			VarTranslator(int from, int to) : fFrom(from), fTo(to) {}

	bool	operator()(Stmt *s);
	bool	operator()(Expr *e);
private:
	int		fFrom;
	int		fTo;
};


class Translatable
{
public:
	virtual void	Translate(int from, int to) = 0;
};


#endif
