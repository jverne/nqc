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

#ifndef __PHashTable_h
#define __PHashTable_h

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif


class PHashable : public PLinkS<PHashable> {
public:
	virtual	~PHashable();

	bool		MatchKey(const char *key);
	const char*	GetKey() const { return fKey; }

protected:
	char*	fKey;
};


class P_HashTable {
public:
				P_HashTable(int size);
				~P_HashTable();

	bool		Remove(PHashable *item);
	void		Add(PHashable *item);

	void		DeleteAll();

	int			Hash(const char *key);

	int			GetBucketCount() const	{ return fSize; }

protected:
	PHashable*	_GetBucket(int i)	{ return fBuckets[i].GetHead(); }
	PHashable*	_Find(const char *key);

private:
	int					fSize;
	PListSS<PHashable>*	fBuckets;
};


template <class T> class PHashTable : public P_HashTable
{
public:
				PHashTable(int size) : P_HashTable(size) {}

	T*			Find(const char *key)	{ return (T*) _Find(key); }
	T*			GetBucket(int i)		{ return (T*) _GetBucket(i); }
};


#endif
