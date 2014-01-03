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

#ifndef __PListS_h
#define __PListS_h

#ifndef nil
#define nil 0L
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

/*

	A generic single linked list class template

	PListS has both head and tail pointers
	PListSS has only a head pointer

	"Linkable" classes should derive from PLinkS<classname>.  For example...

	class Foo: public PLinkS<Foo>
	{
		....
	}


	This makes the Foo class a derivation of PLinkS<Foo>.  If you do
	this then you can use the objects themselves to walk through
	the list...

		PListS<Foo>	list;
		Foo *f;

		for(f=list.GetHead(); f; f=f->GetNext())
			...

	Public calls for PLinkS<T>
		T*		GetNext();

	Public calls for PListS<T>
		void	InsertHead(P_Link *link);
		void	InsertTail(P_Link *link);
		void	Remove(P_Link *link);
		int		IsEmpty() const;
		T*		GetHead() const;
		T*		GetTail() const;
		T*		RemoveHead();
*/

class P_LinkS {
public:
				P_LinkS()	{ fNext = nil; }

	P_LinkS*	_GetNext() const	{ return fNext; }

private:
	P_LinkS*	fNext;

friend class P_ListS;
friend class P_ListSS;
};


class P_ListS {
public:
	void		InsertHead(P_LinkS* link);
	void		InsertTail(P_LinkS* link);
	bool		Remove(P_LinkS* link);

	int			IsEmpty() const		{ return fHead ? 0 : 1; }

protected:
				P_ListS()	{ fHead = nil; fTail = nil; }

	P_LinkS*	_GetHead() const	{ return fHead; }
	P_LinkS*	_GetTail() const { return fTail; }

	P_LinkS*	_RemoveHead();

private:
	P_LinkS*	fHead;
	P_LinkS*	fTail;
};



class P_ListSS {
public:
	void		InsertHead(P_LinkS* link);
	bool		Remove(P_LinkS* link);

	int			IsEmpty() const		{ return fHead ? 0 : 1; }

protected:
				P_ListSS()	{ fHead = nil; }

	P_LinkS*	_GetHead() const	{ return fHead; }

	P_LinkS*	_RemoveHead();

private:
	P_LinkS*	fHead;
};

template <class T> class PLinkS : public P_LinkS
{
public:
	T*		GetNext() 		{ return (T*)_GetNext(); }
	const T*	GetNext() const { return (T*)_GetNext(); }
};


template <class T> class PListS : public P_ListS
{
public:
	T*		GetHead() const	{ return (T*)_GetHead(); }
	T*		GetTail() const	{ return (T*)_GetTail(); }
	T*		RemoveHead()	{ return (T*)_RemoveHead(); }
};


template <class T> class PListSS : public P_ListSS
{
public:
	T*		GetHead() const	{ return (T*)_GetHead(); }
	T*		RemoveHead()	{ return (T*)_RemoveHead(); }
};

#endif
