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
#include "PListS.h"

void P_ListS::InsertHead(P_LinkS* link)
{
	link->fNext = fHead;
	fHead = link;
	if (!fTail)
		fTail = link;
}


void P_ListS::InsertTail(P_LinkS* link)
{
	link->fNext = nil;
	if (fTail)
		fTail->fNext = link;
	else
		fHead = link;
	fTail = link;
}


P_LinkS *P_ListS::_RemoveHead()
{
	P_LinkS *link = fHead;

	if (link)
	{
		fHead = link->fNext;
		link->fNext = nil;
		if (fHead==nil)
			fTail = nil;
	}

	return link;
}


P_LinkS* P_ListSS::_RemoveHead()
{
	P_LinkS *link = fHead;

	if (link)
	{
		fHead = link->fNext;
		link->fNext = nil;
	}

	return link;
}


void P_ListSS::InsertHead(P_LinkS* link)
{
	link->fNext = fHead;
	fHead = link;
}


bool P_ListSS::Remove(P_LinkS *link)
{
	P_LinkS* prev = nil;
	P_LinkS* l;

	for(l=fHead; l; l=l->fNext)
	{
		if (l==link)
		{
			// unlink and return
			if (prev)
				prev->fNext = l->fNext;
			else
				fHead = l->fNext;
			l->fNext = nil;
			return true;
		}
		prev = l;
	}

	return false;
}


bool P_ListS::Remove(P_LinkS *link)
{
	P_LinkS* prev = nil;
	P_LinkS* l;

	for(l=fHead; l; l=l->fNext)
	{
		if (l==link)
		{
			// unlink and return
			if (prev)
				prev->fNext = l->fNext;
			else
				fHead = l->fNext;
			if (fTail==link)
				fTail = prev;
			l->fNext = nil;
			return true;
		}
		prev = l;
	}

	return false;
}
