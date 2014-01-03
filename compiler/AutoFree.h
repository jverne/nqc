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
