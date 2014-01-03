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
