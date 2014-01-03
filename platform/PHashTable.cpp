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
#include "PHashTable.h"

#ifdef DEBUG
using std::strcmp;
#endif

PHashable::~PHashable() {
}


bool PHashable::MatchKey(const char *key) {
	return strcmp(key, fKey)==0 ? true : false;
}


P_HashTable::P_HashTable(int size) {
	fBuckets = new PListSS<PHashable>[size];
	fSize = size;
}


P_HashTable::~P_HashTable() {
	delete [] fBuckets;
}


PHashable* P_HashTable::_Find(const char *key) {
	PHashable *ptr;
	int hash = Hash(key);

	for(ptr=fBuckets[hash].GetHead(); ptr; ptr=ptr->GetNext()) {
		if (ptr->MatchKey(key)) return ptr;
	}

	return nil;
}


void P_HashTable::Add(PHashable *item) {
	int hash = Hash(item->GetKey());

	fBuckets[hash].InsertHead(item);
}


bool P_HashTable::Remove(PHashable *item) {
	int hash = Hash(item->GetKey());

	return fBuckets[hash].Remove(item);
}


void P_HashTable::DeleteAll() {
	int i;
	PHashable *item;

	for(i=0; i<fSize; i++) {
		while((item = fBuckets[i].RemoveHead()) != nil)
			delete item;
	}
}


int P_HashTable::Hash(const char *string)
{
	register unsigned long h=0,g;
	register const char *p;

	for(p=string; *p != 0; p++)
	{
		h = (h <<4) + (*p);
		if ((g= h & 0xf0000000) != 0)
		{
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	return (int)(h % fSize);
}

