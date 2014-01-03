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

