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
#ifndef __AutoFree_h
#define __AutoFree_h

#include <cstddef>

using std::size_t;

class AutoFreeGroup
{
public:
			AutoFreeGroup();

	// these are called by new/delete operators
	void*	allocate(size_t);
	void	free(void*);

	// call this to release all remaining objects
	void	freeAll();

private:
	struct Link
	{
		Link*	next_;
		Link*	prev_;
	};

	// internal call to unlink and dispose an item
	void	dispose(Link *link);


	Link head_;
	Link tail_;
};


extern AutoFreeGroup& GetAutoFreeGroup();

class AutoFree
{
public:
	void* operator new(size_t n)			{ return GetAutoFreeGroup().allocate(n); }
	void operator delete(void* ptr, size_t)	{ GetAutoFreeGroup().free(ptr); }
	void* operator new[](size_t n)			{ return GetAutoFreeGroup().allocate(n); }
	void operator delete[](void * ptr, size_t)	{ GetAutoFreeGroup().free(ptr); }
};

#endif
