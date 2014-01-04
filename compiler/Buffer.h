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
#ifndef _STDIO_H
#include <cstdio>
using std::FILE;
#endif

class Buffer
{
public:
					Buffer();
	virtual			~Buffer();

	bool			Create(const char *name, const char *pathname);
	void			Create(const char *name, FILE *fp);
	void			Create(const char *name, const char *data, int length);

	const char*		GetName() const		{ return fName; }
	int				GetLength() const	{ return fLength; }
	const char *	GetData() const		{ return fData; }

	const char *	GetLine(int line) const;

	int				FindLine(int &offset) const;

	int				FindStartOfLine(int offset) const;
	int				FindEndOfLine(int offset) const;

protected:
	void	FinishCreate(const char *name);
	void	ConvertLineEndings();

	char*	fName;
	char*	fData;
	int		fLength;
};
