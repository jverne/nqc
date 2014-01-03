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
